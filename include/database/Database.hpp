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

template<typename T>
class Database {
 public:
    explicit Database(const std::string& filename)
        : filename(filename) {}

    ~Database() = default;

    void setAdd(const std::string& setName, const T& value) {
        if (sets.find(setName) == sets.end()) {
            sets[setName] = Set<T>();
        }
        sets[setName].insert(value);
    }

    void setRem(const std::string& setName, const T& value) {
        auto it = sets.find(setName);
        if (it == sets.end()) {
            throw std::runtime_error("Set '" + setName + "' not found");
        }
        it->second.remove(value);
    }

    bool setIsMember(const std::string& setName, const T& value) const {
        auto it = sets.find(setName);
        if (it == sets.end()) {
            throw std::runtime_error("Set '" + setName + "' not found");
        }
        return it->second.contains(value);
    }

    void stackPush(const std::string& stackName, const T& value) {
        if (stacks.find(stackName) == stacks.end()) {
            stacks[stackName] = Stack<T>();
        }
        stacks[stackName].push(value);
    }

    T stackPop(const std::string& stackName) {
        auto it = stacks.find(stackName);
        if (it == stacks.end() || it->second.getSize() == 0) {
            throw std::runtime_error("Stack '" + stackName + "' is empty or not found");
        }

        T value = it->second.peek();
        it->second.pop();
        return value;
    }

    void queuePush(const std::string& queueName, const T& value) {
        if (queues.find(queueName) == queues.end()) {
            queues[queueName] = myQueue<T>();
        }
        queues[queueName].push(value);
    }

    T queuePop(const std::string& queueName) {
        auto it = queues.find(queueName);
        if (it == queues.end() || it->second.getSize() == 0) {
            throw std::runtime_error("Queue '" + queueName + "' is empty or not found");
        }

        T value = it->second.front();
        it->second.pop();
        return value;
    }

    void hashSet(const std::string& hashName, const std::string& key, const T& value) {
        if (hashes.find(hashName) == hashes.end()) {
            hashes[hashName] = HashTableOA<std::string, T>(1000);
        }
        hashes[hashName].insert(key, value);
    }

    void hashDel(const std::string& hashName, const std::string& key) {
        auto it = hashes.find(hashName);
        if (it == hashes.end()) {
            throw std::runtime_error("Hash '" + hashName + "' not found");
        }
        it->second.remove(key);
    }

    T hashGet(const std::string& hashName, const std::string& key) const {
        auto it = hashes.find(hashName);
        if (it == hashes.end()) {
            throw std::runtime_error("Hash '" + hashName + "' not found");
        }
        return it->second.find(key);
    }


    void load() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;  // файл не существует - создастчя при save()
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
        file << "# Format: name:type|data\n\n";

        for (const auto& [name, hash] : hashes) {
            file << name << ":HASH|";
            savePairs(file, hash);
            file << "\n";
        }

        for (const auto& [name, set] : sets) {
            file << name << ":SET|";
            saveElements(file, set);
            file << "\n";
        }

        for (const auto& [name, stack] : stacks) {
            file << name << ":STACK|";
            saveElements(file, stack);
            file << "\n";
        }

        for (const auto& [name, queue] : queues) {
            file << name << ":QUEUE|";
            saveElements(file, queue);
            file << "\n";
        }

        file.close();
    }

 private:
    std::string filename;

    std::map<std::string, Set<T>> sets;
    std::map<std::string, Stack<T>> stacks;
    std::map<std::string, myQueue<T>> queues;
    std::map<std::string, HashTableOA<std::string, T>> hashes;

    void loadSet(const std::string& name, const std::string& data) {
        sets[name] = Set<T>();
        std::vector<std::string> elements = StringUtils::split(data, '|');
        for (const auto& elem : elements) {
            if (!elem.empty()) {
                sets[name].insert(StringUtils::parseValue<T>(elem));
            }
        }
    }

    void loadHash(const std::string& name, const std::string& data) {
        if (hashes.find(name) == hashes.end()) {
            hashes.emplace(name, HashTableOA<std::string, T>(1000));
        }

        if (data.empty()) return;

        std::vector<std::string> pairs = StringUtils::split(data, '|');
        for (const auto& pair : pairs) {
            if (pair.empty()) continue;
            size_t colonPos = pair.find(':');
            if (colonPos != std::string::npos) {
                std::string key = pair.substr(0, colonPos);
                std::string valueStr = pair.substr(colonPos + 1);
                T value = StringUtils::parseValue<T>(valueStr);
                hashes[name].insert(key, value);
            }
        }
    }

    void loadStack(const std::string& name, const std::string& data) {
        stacks[name] = Stack<T>();
        std::vector<std::string> elements = StringUtils::split(data, '|');

        // в обратном порядке
        for (int i = elements.size() - 1; i >= 0; --i) {
            if (!elements[i].empty()) {
                stacks[name].push(StringUtils::parseValue<T>(elements[i]));
            }
        }
    }

    void loadQueue(const std::string& name, const std::string& data) {
        queues[name] = myQueue<T>();
        std::vector<std::string> elements = StringUtils::split(data, '|');
        for (const auto& elem : elements) {
            if (!elem.empty()) {
                queues[name].push(StringUtils::parseValue<T>(elem));
            }
        }
    }

    // вспомогательные методы для сохранения
    void savePairs(std::ostream& out, const HashTableOA<std::string, T>& hash) const {
        hash.savePairsToStream(out);
    }

    void saveElements(std::ostream& out, const Set<T>& set) const {
        set.saveElementsToStream(out);
    }

    void saveElements(std::ostream& out, const Stack<T>& stack) const {
        stack.saveElementsToStream(out);
    }

    void saveElements(std::ostream& out, const myQueue<T>& queue) const {
        queue.saveElementsToStream(out);
    }
};
