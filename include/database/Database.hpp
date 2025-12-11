// Copyright
#pragma once

#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../containers/Set.hpp"
#include "../containers/Stack.hpp"
#include "../containers/Queue.hpp"
#include "../containers/HashTableOA.hpp"
#include "../utils/StringUtils.hpp"

// Типы данных, которые может хранить БД
enum class DataType {
    STRING,      // std::string
    INTEGER,     // int
    FLOAT,       // float
    UNKNOWN
};

// Преобразование строки в enum
inline DataType stringToDataType(const std::string& str) {
    std::string lower = StringUtils::toLower(str);
    if (lower == "string") return DataType::STRING;
    if (lower == "int" || lower == "integer") return DataType::INTEGER;
    if (lower == "float" || lower == "double") return DataType::FLOAT;
    return DataType::UNKNOWN;
}

// Преобразование enum в строку
inline std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::STRING: return "STRING";
        case DataType::INTEGER: return "INTEGER";
        case DataType::FLOAT: return "FLOAT";
        default: return "UNKNOWN";
    }
}

class Database {
 public:
    explicit Database(const std::string& filename, DataType type = DataType::STRING)
        : filename(filename), dataType(type) {}

    ~Database() = default;

    DataType getDataType() const {
        return dataType;
    }

    std::string getDataTypeString() const {
        return dataTypeToString(dataType);
    }

    // ===== SET ОПЕРАЦИИ =====
    void setAdd(const std::string& setName, const std::string& value) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("sAdd only works with STRING data type");
        }
        if (sets.find(setName) == sets.end()) {
            sets[setName] = Set<std::string>();
        }
        sets[setName].insert(value);
    }

    void setRem(const std::string& setName, const std::string& value) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("sRem only works with STRING data type");
        }
        if (sets.find(setName) == sets.end()) {
            throw std::runtime_error("Set '" + setName + "' not found");
        }
        sets[setName].remove(value);
    }

    bool setIsMember(const std::string& setName, const std::string& value) const {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("sIsMember only works with STRING data type");
        }
        auto it = sets.find(setName);
        if (it == sets.end()) {
            throw std::runtime_error("Set '" + setName + "' not found");
        }
        return it->second.contains(value);
    }

    // ===== STACK ОПЕРАЦИИ =====
    void stackPush(const std::string& stackName, const std::string& value) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("sPush only works with STRING data type");
        }
        if (stacks.find(stackName) == stacks.end()) {
            stacks[stackName] = Stack<std::string>();
        }
        stacks[stackName].push(value);
    }

    std::string stackPop(const std::string& stackName) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("sPop only works with STRING data type");
        }
        auto it = stacks.find(stackName);
        if (it == stacks.end() || it->second.getSize() == 0) {
            throw std::runtime_error("Stack '" + stackName + "' is empty or not found");
        }

        std::string value = it->second.peek();
        it->second.pop();
        return value;
    }

    // ===== QUEUE ОПЕРАЦИИ =====
    void queuePush(const std::string& queueName, const std::string& value) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("qPush only works with STRING data type");
        }
        if (queues.find(queueName) == queues.end()) {
            queues[queueName] = myQueue<std::string>();
        }
        queues[queueName].push(value);
    }

    std::string queuePop(const std::string& queueName) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("qPop only works with STRING data type");
        }
        auto it = queues.find(queueName);
        if (it == queues.end() || it->second.getSize() == 0) {
            throw std::runtime_error("Queue '" + queueName + "' is empty or not found");
        }

        std::string value = it->second.front();
        it->second.pop();
        return value;
    }

    // ===== HASH ОПЕРАЦИИ =====
    void hashSet(const std::string& hashName, const std::string& key, const std::string& value) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("hSet only works with STRING data type");
        }
        if (hashes.find(hashName) == hashes.end()) {
            hashes[hashName] = HashTableOA<std::string, std::string>(1000);
        }
        hashes[hashName].insert(key, value);
    }

    void hashDel(const std::string& hashName, const std::string& key) {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("hDel only works with STRING data type");
        }
        auto it = hashes.find(hashName);
        if (it == hashes.end()) {
            throw std::runtime_error("Hash '" + hashName + "' not found");
        }
        it->second.remove(key);
    }

    std::string hashGet(const std::string& hashName, const std::string& key) const {
        if (dataType != DataType::STRING) {
            throw std::runtime_error("hGet only works with STRING data type");
        }
        auto it = hashes.find(hashName);
        if (it == hashes.end()) {
            throw std::runtime_error("Hash '" + hashName + "' not found");
        }
        return it->second.find(key);
    }

    // ===== ФАЙЛОВЫЕ ОПЕРАЦИИ =====
    void load() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;  // Файл не существует - создадим при save()
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            size_t colonPos = line.find(':');
            size_t pipePos = line.find('|');

            if (colonPos == std::string::npos || pipePos == std::string::npos) {
                continue;
            }

            std::string name = line.substr(0, colonPos);
            std::string type = line.substr(colonPos + 1, pipePos - colonPos - 1);
            std::string data = line.substr(pipePos + 1);

            if (type == "SET") {
                loadSet(name, data);
            } else if (type == "HASH") {
                loadHash(name, data);
            } else if (type == "STACK") {
                loadStack(name, data);
            } else if (type == "QUEUE") {
                loadQueue(name, data);
            }
        }

        file.close();
    }

    void save() {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }

        file << "# СУБД Data File\n";
        file << "# Data Type: " << getDataTypeString() << "\n";
        file << "# Format: name:type|data\n\n";

        // Сохраняем множества
        for (const auto& [name, set] : sets) {
            file << name << ":SET|";
            // TODO: добавить итератор в Set
            file << "\n";
        }

        // Сохраняем хеш-таблицы
        for (const auto& [name, hash] : hashes) {
            file << name << ":HASH|";
            hash.savePairsToStream(file);
            file << "\n";
        }

        // Сохраняем стеки
        for (const auto& [name, stack] : stacks) {
            file << name << ":STACK|";
            // TODO: добавить метод для получения всех элементов
            file << "\n";
        }

        // Сохраняем очереди
        for (const auto& [name, queue] : queues) {
            file << name << ":QUEUE|";
            // TODO: добавить метод для получения всех элементов
            file << "\n";
        }

        file.close();
    }

 private:
    std::string filename;
    DataType dataType;

    std::map<std::string, Set<std::string>> sets;
    std::map<std::string, Stack<std::string>> stacks;
    std::map<std::string, myQueue<std::string>> queues;
    std::map<std::string, HashTableOA<std::string, std::string>> hashes;

    void loadSet(const std::string& name, const std::string& data) {
        sets[name] = Set<std::string>();
        std::vector<std::string> elements = StringUtils::split(data, '|');
        for (const auto& elem : elements) {
            if (!elem.empty()) {
                sets[name].insert(elem);
            }
        }
    }

    void loadHash(const std::string& name, const std::string& data) {
        hashes[name] = HashTableOA<std::string, std::string>(1000);
        std::vector<std::string> pairs = StringUtils::split(data, '|');
        for (const auto& pair : pairs) {
            if (pair.empty()) continue;
            size_t colonPos = pair.find(':');
            if (colonPos != std::string::npos) {
                std::string key = pair.substr(0, colonPos);
                std::string value = pair.substr(colonPos + 1);
                hashes[name].insert(key, value);
            }
        }
    }

    void loadStack(const std::string& name, const std::string& data) {
        stacks[name] = Stack<std::string>();
        std::vector<std::string> elements = StringUtils::split(data, '|');
        for (const auto& elem : elements) {
            if (!elem.empty()) {
                stacks[name].push(elem);
            }
        }
    }

    void loadQueue(const std::string& name, const std::string& data) {
        queues[name] = myQueue<std::string>();
        std::vector<std::string> elements = StringUtils::split(data, '|');
        for (const auto& elem : elements) {
            if (!elem.empty()) {
                queues[name].push(elem);
            }
        }
    }
};
