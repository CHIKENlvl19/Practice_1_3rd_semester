// Copyright
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

class StringUtils {
 public:
    // разбить строку по разделителю
    static std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

    // разбить строку с учетом кавычек
    static std::vector<std::string> splitWithQuotes(const std::string& str) {
        std::vector<std::string> result;
        std::string current;
        bool inQuotes = false;

        for (size_t i = 0; i < str.length(); ++i) {
            char c = str[i];
            if (c == '"' || c == '\'') {
                inQuotes = !inQuotes;
            } else if (c == ' ' && !inQuotes) {
                if (!current.empty()) {
                    result.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            result.push_back(current);
        }

        return result;
    }

    static std::string toLower(const std::string& str) {
        std::string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }


    // парсинг из строки в тип T
    template<typename T>
    static T parseValue(const std::string& s);

    // преобразование типа T в строку
    template<typename T>
    static std::string toStringValue(const T& v);
};

// специализации для std::string
template<>
inline std::string StringUtils::parseValue<std::string>(const std::string& s) {
    return s;
}

template<>
inline std::string StringUtils::toStringValue<std::string>(const std::string& v) {
    return v;
}

// специализации для int
template<>
inline int StringUtils::parseValue<int>(const std::string& s) {
    try {
        size_t pos = 0;
        int v = std::stoi(s, &pos);
        if (pos != s.size()) {
            throw std::invalid_argument("extra characters after number");
        }
        return v;
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid integer: '" + s + "'");
    }
}

template<>
inline std::string StringUtils::toStringValue<int>(const int& v) {
    return std::to_string(v);
}

// специализации для float
template<>
inline float StringUtils::parseValue<float>(const std::string& s) {
    try {
        size_t pos = 0;
        float v = std::stof(s, &pos);
        if (pos != s.size()) {
            throw std::invalid_argument("extra characters after number");
        }
        return v;
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid float: '" + s + "'");
    }
}

template<>
inline std::string StringUtils::toStringValue<float>(const float& v) {
    return std::to_string(v);
}
