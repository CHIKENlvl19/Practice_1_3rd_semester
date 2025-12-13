#include <iostream>
#include <string>
#include <cstring>
#include "database/Database.hpp"
#include "database/CommandParser.hpp"

using namespace std;

// типы данных
enum class DataType {
    STRING,
    INTEGER,
    FLOAT,
    UNKNOWN
};

DataType stringToDataType(const std::string& str) {
    std::string lower = str;
    for (char& c : lower) c = std::tolower(c);

    if (lower == "string") return DataType::STRING;
    if (lower == "int" || lower == "integer") return DataType::INTEGER;
    if (lower == "float" || lower == "double") return DataType::FLOAT;
    return DataType::UNKNOWN;
}

std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::STRING: return "STRING";
        case DataType::INTEGER: return "INTEGER";
        case DataType::FLOAT: return "FLOAT";
        default: return "UNKNOWN";
    }
}

// шаблонная функция для выполнения запроса
template<typename T>
void runQuery(const string& filename, const string& query) {
    Database<T> db(filename);
    db.load();

    CommandParser<T> parser(db);
    CommandResult result = parser.execute(query);

    if (result.success) {
        if (!result.output.empty()) {
            cout << result.output << "\n";
        }
        db.save();
    } else {
        cerr << "Error: " << result.error << "\n";
        throw runtime_error(result.error);
    }
}

int main(int argc, char* argv[]) {
    string filename;
    string query;
    string dataTypeStr = "string";  // по умолчанию STRING

    // парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[++i];
        } else if (strcmp(argv[i], "--type") == 0 && i + 1 < argc) {
            dataTypeStr = argv[++i];
        }
    }

    // валидация
    if (filename.empty()) {
        cout << "Usage: ./dbms --file <filename> --query '<command>' [--type <type>]\n";
        cout << "\nTypes: string (default), int, float\n";
        cout << "\nExamples:\n";
        cout << "  ./dbms --file data.data --query 'HSET users name Alice'\n";
        cout << "  ./dbms --file nums.data --query 'HSET scores player1 100' --type int\n";
        cout << "  ./dbms --file temps.data --query 'HSET metrics temp 36.6' --type float\n";
        return 1;
    }

    if (query.empty()) {
        cerr << "Error: --query is required\n";
        return 1;
    }

    DataType dataType = stringToDataType(dataTypeStr);

    try {
        // выбираем тип базы данных в зависимости от --type
        if (dataType == DataType::STRING) {
            runQuery<std::string>(filename, query);
        } else if (dataType == DataType::INTEGER) {
            runQuery<int>(filename, query);
        } else if (dataType == DataType::FLOAT) {
            runQuery<float>(filename, query);
        } else {
            cerr << "Error: Unknown data type '" << dataTypeStr << "'\n";
            cerr << "Valid types: string, int, float\n";
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
