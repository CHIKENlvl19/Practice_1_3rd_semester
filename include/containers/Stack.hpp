// Copyright message
#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <string>

const int MAX_STACK_SIZE = 1000000;

template <typename T>
class Stack {
 public:
    Stack() : head(nullptr), size(0) {}

    Stack(const Stack& other) : head(nullptr), size(0) {
        if (!other.head) {
            return;
        }

        std::vector<T> buf;
        Node* cur = other.head;
        while (cur) {
            buf.push_back(cur->value);
            cur = cur->next;
        }

        for (int i = static_cast<int>(buf.size()) - 1; i >= 0; --i) {
            head = new Node(buf[i], head);
            size++;
        }
    }

    Stack<T>& operator=(const Stack& other) {
        if (this == &other) {
            return *this;
        }

        clean();

        std::vector<T> buf;
        Node* cur = other.head;
        while (cur) {
            buf.push_back(cur->value);
            cur = cur->next;
        }

        for (int i = static_cast<int>(buf.size()) - 1; i >= 0; --i) {
            head = new Node(buf[i], head);
            size++;
        }

        return *this;
    }

    ~Stack() {
        clean();
    }

    void push(const T& value) {
        if (size >= MAX_STACK_SIZE)
            throw std::overflow_error("Error: stack is full!");

        head = new Node(value, head);
        size++;
    }

    void pop() {
        if (size == 0)
            throw std::underflow_error("Error: stack is empty!");

        Node* toDelete = head;
        head = head->next;
        delete toDelete;
        size--;
    }

    T peek() const {
        if (size == 0)
            throw std::underflow_error("Stack is empty!");

        return head->value;
    }

    void print() const {
        if (size == 0) {
            std::cout << "Stack is empty!\n";
            return;
        }

        Node* current = head;
        std::cout << "nullptr";
        while (current) {
            std::cout << " <- " << current->value;
            current = current->next;
        }
        std::cout << "\n";
    }

    int getSize() const {
        return size;
    }

    // текстовый формат
    void saveText(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }

        file << size << "\n";

        if (size > 0) {
            T* elements = new T[size];
            Node* cur = head;
            int idx = 0;
            while (cur) {
                elements[idx++] = cur->value;
                cur = cur->next;
            }

            for (int i = size - 1; i >= 0; --i) {
                file << elements[i];
                if (i > 0) file << " ";
            }
            delete[] elements;
        }
        file.close();
    }

    void loadText(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        clean();

        int newSize;
        file >> newSize;

        if (newSize > 0) {
            T* elements = new T[newSize];
            for (int i = 0; i < newSize; ++i) {
                file >> elements[i];
            }

            for (int i = 0; i < newSize; ++i) {
                push(elements[i]);
            }
            delete[] elements;
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

        if (size > 0) {
            T* elements = new T[size];
            Node* cur = head;
            int idx = 0;
            while (cur) {
                elements[idx++] = cur->value;
                cur = cur->next;
            }

            for (int i = size - 1; i >= 0; --i) {
                file.write(reinterpret_cast<const char*>(&elements[i])
                    , sizeof(T));
            }
            delete[] elements;
        }
        file.close();
    }

    void loadBinary(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        clean();

        int newSize;
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));

        if (newSize > 0) {
            T* elements = new T[newSize];
            for (int i = 0; i < newSize; ++i) {
                file.read(reinterpret_cast<char*>(&elements[i]), sizeof(T));
            }

            for (int i = 0; i < newSize; ++i) {
                push(elements[i]);
            }
            delete[] elements;
        }

        file.close();
    }



 private:
    struct Node {
        T value;
        Node* next;

        explicit Node(const T& v, Node* ns = nullptr)
            : value(v), next(ns) {}
    };

    void clean() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        size = 0;
    }

    Node* head;
    int size;
};
