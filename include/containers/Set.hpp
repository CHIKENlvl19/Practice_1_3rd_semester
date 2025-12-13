// Copyright
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../containers/HashTableOA.hpp"
#include "../utils/StringUtils.hpp"

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

    void saveElementsToStream(std::ostream& out) const {
        table.saveKeysToStream(out);
    }

 private:
    HashTableOA<T, bool> table;
};
