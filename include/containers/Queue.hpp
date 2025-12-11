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

    void saveElementsToStream(std::ostream& out) const {
        // от head к tail
        for (int i = 0; i < size; ++i) {
            out << data[(head + i) % capacity] << "|";
        }
    }


 private:
    T* data;
    int head;
    int tail;
    int size;
    int capacity;
};
