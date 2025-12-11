// Copyright message
#pragma once
#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <type_traits>
#include <fstream>

template <typename Key, typename Value>
class HashTableOA {
 public:
    explicit HashTableOA(int capacity)
        : size(0), capacity(capacity), loadFactor(0.0f), p(1000000007) {
        table = new Cell[capacity];
        init();
    }

    HashTableOA() : HashTableOA(1000) {}  // конструктор по умолчанию


    HashTableOA(const HashTableOA& other)
        : size(other.getSize()),
          capacity(other.getCapacity()),
          loadFactor(other.getLoadFactor()),
          a(other.a),
          b(other.b),
          p(other.p) {
            table = new Cell[other.getCapacity()];
            for (size_t i = 0; i < capacity; i++) {
                table[i] = other.table[i];
            }
          }

    HashTableOA& operator=(const HashTableOA& other) {
        if (this != &other) {
            HashTableOA<Key, Value> tmp(other);
            swap(tmp);
        }

        return *this;
    }

    ~HashTableOA() {
        clean();
    }

    bool insert(const Key& key, const Value& value) {
        size_t index;
        for (size_t i = 0; i < capacity; i++) {
            index = (h1(key) + i) % capacity;

            if (!table[index].isOccupied || table[index].isDeleted) {
                table[index].key = key;
                table[index].value = value;
                table[index].isOccupied = true;
                table[index].isDeleted = false;
                size++;
                loadFactor = getLoadFactor();
                return true;
            }

            if (table[index].isOccupied && !table[index].isDeleted
                    && table[index].key == key) {
                table[index].value = value;
                return true;
            }
        }

        std::cerr << "Error: table is full!" << std::endl;
        return false;
    }


    bool isPresent(const Key& key) const {
        size_t h = h1(key);

        for (size_t i = 0; i < capacity; i++) {
            size_t index = (h + i) % capacity;
            const Cell& cell = table[index];

            if (!cell.isOccupied && !cell.isDeleted) {
                return false;
            }
            if (cell.isOccupied && !cell.isDeleted && cell.key == key) {
                return true;
            }
        }

        return false;
    }


    Value find(const Key& key) const {
        size_t h = h1(key);

        for (size_t i = 0; i < capacity; i++) {
            size_t index = (h + i) % capacity;

            const Cell& cell = table[index];

            if (!cell.isOccupied && !cell.isDeleted) {
                return Value();
            }

            if (cell.isOccupied && !cell.isDeleted && cell.key == key) {
                return cell.value;
            }
        }

        return Value();
    }


    bool remove(const Key& key) {
        size_t index;
        for (size_t i = 0; i < capacity; i++) {
            index = (h1(key) + i) % capacity;
            if (!table[index].isOccupied && !table[index].isDeleted) {
                return false;
            }
            if (table[index].isOccupied && table[index].key == key) {
                table[index].isDeleted = true;
                table[index].isOccupied = false;
                size--;
                loadFactor = getLoadFactor();
                return true;
            }
        }
        return false;
    }


    void print() const {
        for (size_t i = 0; i < capacity; i++) {
            std::cout << "[" << i << "]";
            if (table[i].isOccupied) {
                std::cout << " {" << table[i].key
                    << ": " << table[i].value << "}";
            } else if (table[i].isDeleted) {
                std::cout << "(deleted)";
            }
            std::cout << std::endl;
        }
    }

    void clean() {
        delete[] table;
        table = nullptr;
        size = 0;
        loadFactor = 0.0f;
    }


    size_t getSize() const {
        return size;
    }


    size_t getCapacity() const {
        return capacity;
    }


    float getLoadFactor() const {
        if (capacity == 0) {
            return 0.0f;
        }

        return static_cast<float>(size) / capacity;
    }

    void savePairsToStream(std::ostream& out) const {
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i].isOccupied && !table[i].isDeleted) {
                out << table[i].key << ":" << table[i].value << "|";
            }
        }
    }

    // текстовый формат
    void saveText(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }

        file << size << " " << capacity << " " << a << " " << b << " " << p << "\n";

        for (size_t i = 0; i < capacity; i++) {
            if (table[i].isOccupied && !table[i].isDeleted) {
                file << table[i].key << " " << table[i].value << "\n";
            }
        }
        file.close();
    }

    void loadText(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        delete[] table;

        size_t oldSize;
        file >> oldSize >> capacity >> a >> b >> p;

        table = new Cell[capacity];
        size = 0;
        loadFactor = 0.0f;

        for (size_t i = 0; i < oldSize; ++i) {
            Key key;
            Value value;
            file >> key >> value;
            insert(key, value);
        }
        file.close();
    }


    // бинарный формат
    void saveBinary(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }

        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
        file.write(reinterpret_cast<const char*>(&a), sizeof(a));
        file.write(reinterpret_cast<const char*>(&b), sizeof(b));
        file.write(reinterpret_cast<const char*>(&p), sizeof(p));

        for (size_t i = 0; i < capacity; i++) {
            if (table[i].isOccupied && !table[i].isDeleted) {
                file.write(reinterpret_cast<const char*>(&table[i].key), sizeof(Key));
                file.write(reinterpret_cast<const char*>(&table[i].value), sizeof(Value));
            }
        }
        file.close();
    }

    void loadBinary(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        delete[] table;

        size_t oldSize;
        file.read(reinterpret_cast<char*>(&oldSize), sizeof(oldSize));
        file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
        file.read(reinterpret_cast<char*>(&a), sizeof(a));
        file.read(reinterpret_cast<char*>(&b), sizeof(b));
        file.read(reinterpret_cast<char*>(&p), sizeof(p));

        table = new Cell[capacity];
        size = 0;
        loadFactor = 0.0f;

        for (size_t i = 0; i < oldSize; ++i) {
            Key key;
            Value value;
            file.read(reinterpret_cast<char*>(&key), sizeof(Key));
            file.read(reinterpret_cast<char*>(&value), sizeof(Value));
            insert(key, value);
        }
        file.close();
    }


 private:
    struct Cell {
        Key key;
        Value value;
        bool isOccupied;
        bool isDeleted;

        Cell() : isOccupied(false), isDeleted(false) {}
    };

    Cell* table;
    size_t size;
    size_t capacity;
    float loadFactor;

    int a, b;
    int p;

    void init() {
        std::mt19937 gen(1337);
        std::uniform_int_distribution<int> dist(1, 1000);
        a = dist(gen);
        b = dist(gen);
    }

    size_t h1(const Key& key) const {
        uint64_t keyValue = 0;

        if constexpr (std::is_integral_v<Key> || std::is_floating_point_v<Key>) {
            keyValue = static_cast<uint64_t>(key);
        } else {
            for (char c : key) {
                keyValue = keyValue * 131 + static_cast<unsigned char>(c);
            }
        }

        return static_cast<size_t>(((a * keyValue + b) % p) % capacity);
    }

    void swap(HashTableOA& other) noexcept {
        std::swap(table, other.table);
        std::swap(loadFactor, other.loadFactor);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);

        std::swap(a, other.a);
        std::swap(b, other.b);
        std::swap(p, other.p);
    }
};
