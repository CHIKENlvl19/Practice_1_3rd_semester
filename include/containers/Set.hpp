// Copyright
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../containers/HashTableOA.hpp"

template <typename T>
class Set {
 public:
    Set() : table(1000) {}

    explicit Set(int capacity) : table(capacity) {}

    bool insert(const T& value) {
        return table.insert(value, true);
    }

    bool remove(const T& value) {
        return table.remove(value);
    }

    bool contains(const T& value) const {
        return table.isPresent(value);
    }

    size_t size() const {
        return table.getSize();
    }

    void print() const {
        table.print();
    }

    // сериализация в текстовом формате
    // формат: element1|element2|element3
    std::string serialize() const {
        std::stringstream ss;
        bool first = true;

        // проходим по всей таблице и выводим только занятые ячейки
        for (size_t i = 0; i < table.getCapacity(); ++i) {
            // нужно добавить публичный метод в HashTableOA для итерации
        }

        return ss.str();
    }

    void deserialize(const std::string& data) {
        std::stringstream ss(data);
        std::string element;

        while (std::getline(ss, element, '|')) {
            if (!element.empty()) {
                insert(element);
            }
        }
    }

 private:
    HashTableOA<T, bool> table;
};
