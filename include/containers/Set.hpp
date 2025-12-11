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
    void saveElementsToStream(std::ostream& out) const {
        // Set использует HashTableOA внутри
        // проходим по всем ключам хеш-таблицы
        table.saveKeysToStream(out);
    }

 private:
    HashTableOA<T, bool> table;
};
