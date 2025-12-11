// Copyright message
#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

template <typename T>
class myQueue {
 public:
    explicit myQueue(int initialCapacity = 4)
        : data(nullptr), head(0), tail(0), size(0) {
        if (initialCapacity <= 0) initialCapacity = 4;
        capacity = initialCapacity;
        data = new T[capacity];
    }

    myQueue(const myQueue& other)
        : data(new T[other.capacity]),
          head(0),
          tail(other.size),
          size(other.size),
          capacity(other.capacity) {
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[(other.head + i) % other.capacity];
        }
    }

    myQueue& operator=(const myQueue& other) {
        if (this != &other) {
            delete[] data;

            capacity = other.capacity;
            size = other.size;
            head = 0;
            tail = size;
            data = new T[capacity];

            for (int i = 0; i < size; ++i) {
                data[i] = other.data[(other.head + i) % other.capacity];
            }
        }
        return *this;
    }


    ~myQueue() {
        clean();
    }

    void clean() {
        delete[] data;
        data = nullptr;
        head = tail = size = capacity = 0;
    }

    void push(const T& value) {
        if (size == capacity) {
            int newCapacity = capacity * 2;
            T* newData = new T[newCapacity];
            for (int i = 0; i < size; ++i) {
                newData[i] = data[(head + i) % capacity];
            }

            delete[] data;
            data = newData;
            capacity = newCapacity;
            head = 0;
            tail = size;
        }

        data[tail] = value;
        tail = (tail + 1) % capacity;
        size++;
    }

    void pop() {
        if (size == 0) {
            throw std::underflow_error("Queue is empty!");
        }

        head = (head + 1) % capacity;
        size--;
    }

    void print() const {
        if (size == 0) {
            std::cout << "Queue is empty!" << std::endl;
            return;
        }

        for (int i = 0; i < size; ++i) {
            std::cout << data[(head + i) % capacity] << " ";
        }
        std::cout << std::endl;
    }

    T front() const {
        if (size == 0) {
            throw std::underflow_error("Queue is empty!");
        }

        return data[head];
    }

    int getSize() const {
        return size;
    }

    int getCapacity() const {
        return capacity;
    }

    // текстовый формат
    void saveText(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        out << size << " " << capacity << "\n";
        for (int i = 0; i < size; ++i) {
            out << data[(head + i) % capacity];
            if (i < size - 1) out << " ";
        }
        out.close();
    }

    void loadText(const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        int newSize, newCapacity;
        in >> newSize >> newCapacity;

        delete[] data;
        capacity = newCapacity;
        size = newSize;
        head = 0;
        tail = size % capacity;
        data = new T[capacity];

        for (int i = 0; i < size; ++i) {
            in >> data[i];
        }
        in.close();
    }

    // бинарный формат
    void saveBinary(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));

        for (int i = 0; i < size; ++i) {
            T value = data[(head + i) % capacity];
            file.write(reinterpret_cast<const char*>(&value), sizeof(T));
        }
        file.close();
    }

    void loadBinary(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        int newSize, newCapacity;
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));
        file.read(reinterpret_cast<char*>(&newCapacity), sizeof(newCapacity));

        delete[] data;
        capacity = newCapacity;
        size = newSize;
        head = 0;
        tail = size % capacity;
        data = new T[capacity];

        for (int i = 0; i < size; ++i) {
            file.read(reinterpret_cast<char*>(&data[i]), sizeof(T));
        }
        file.close();
    }


 private:
    T* data;
    int head;
    int tail;
    int size;
    int capacity;
};
