// Copyright
#include <iostream>
#include <string>
#include <cstring>
#include "database/Database.hpp"
#include "database/CommandParser.hpp"

using namespace std;

void printMenu() {
    cout << "\n=== NoSQL СУБД ===" << endl;
    cout << "Выберите тип данных для БД:" << endl;
    cout << "1. STRING (строки) - по умолчанию" << endl;
    cout << "2. INTEGER (целые числа)" << endl;
    cout << "3. FLOAT (вещественные числа)" << endl;
    cout << endl;
}

int main(int argc, char* argv[]) {
    string filename;
    string query;
    string dataTypeStr = "";

    // парсинг аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[++i];
        } else if (strcmp(argv[i], "--type") == 0 && i + 1 < argc) {
            dataTypeStr = argv[++i];
        }
    }

    // если нет файла и query - показать справку
    if (filename.empty()) {
        cout << "Usage: ./dbms --file <filename> --query '<command>' [--type <data_type>]\n";
        cout << "\nДопустимые типы данных:\n";
        cout << "  - string (по умолчанию)\n";
        cout << "  - int / integer\n";
        cout << "  - float / double\n";
        cout << "\nПримеры:\n";
        cout << "  ./dbms --file data.data --query 'HSET myhash key value'\n";
        cout << "  ./dbms --file data.data --query 'HSET myhash key value' --type string\n";
        cout << "  ./dbms --file data.data --query 'SADD myset element'\n";
        return 1;
    }

    // если файл существует, но нет query - интерактивный режим
    if (query.empty()) {
        printMenu();
        int choice;
        cout << "Выберите (1-3): ";
        cin >> choice;
        cin.ignore();  // Очистить буфер ввода

        DataType selectedType = DataType::STRING;
        switch (choice) {
            case 1: selectedType = DataType::STRING; break;
            case 2: selectedType = DataType::INTEGER; break;
            case 3: selectedType = DataType::FLOAT; break;
            default:
                cout << "Неверный выбор. Используется STRING по умолчанию.\n";
                selectedType = DataType::STRING;
        }

        cout << "\nУспешно! Выбран тип: " << dataTypeToString(selectedType) << endl;
        cout << "\nТеперь используйте: ./dbms --file " << filename << " --query '<command>' --type ";

        switch (selectedType) {
            case DataType::STRING: cout << "string"; break;
            case DataType::INTEGER: cout << "int"; break;
            case DataType::FLOAT: cout << "float"; break;
            default: cout << "string";
        }

        cout << "\n\nПримеры команд:\n";
        cout << "  HSET myhash key value    - добавить в хеш\n";
        cout << "  HGET myhash key          - получить из хеша\n";
        cout << "  HDEL myhash key          - удалить из хеша\n";
        cout << "  SADD myset element       - добавить в множество\n";
        cout << "  SREM myset element       - удалить из множества\n";
        cout << "  SISMEMBER myset element  - проверить множество\n";
        cout << "  SPUSH mystack item       - push в стек\n";
        cout << "  SPOP mystack             - pop из стека\n";
        cout << "  QPUSH myqueue elem       - push в очередь\n";
        cout << "  QPOP myqueue             - pop из очереди\n";
        return 0;
    }

    // определить тип данных
    DataType dataType = DataType::STRING;  // по дефолту строка
    if (!dataTypeStr.empty()) {
        dataType = stringToDataType(dataTypeStr);
        if (dataType == DataType::UNKNOWN) {
            cerr << "Error: Unknown data type '" << dataTypeStr << "'\n";
            cerr << "Допустимые типы: string, int, integer, float, double\n";
            return 1;
        }
    }

    try {
        // инициализация БД с выбранным типом данных
        Database db(filename, dataType);
        db.load();

        // парсинг и выполнение команды
        CommandParser parser(db);
        CommandResult result = parser.execute(query);

        if (result.success) {
            if (!result.output.empty()) {
                cout << result.output << "\n";
            }
            db.save();
        } else {
            cerr << "Error: " << result.error << "\n";
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}