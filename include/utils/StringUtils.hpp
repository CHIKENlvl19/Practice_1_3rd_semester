// Copyright
#pragma once

#include <string>
#include <vector>
#include <sstream>

// Утилита для парсинга команд
class StringUtils {
 public:
    // Разбить строку по разделителю
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

    // Разбить строку по разделителю и вернуть вектор
    static std::vector<std::string> splitByPipe(const std::string& str) {
        return split(str, '|');
    }

    // Объединить вектор строк с разделителем
    static std::string join(const std::vector<std::string>& tokens, char delimiter = '|') {
        if (tokens.empty()) return "";

        std::string result = tokens[0];
        for (size_t i = 1; i < tokens.size(); ++i) {
            result += delimiter;
            result += tokens[i];
        }
        return result;
    }

    // Преобразовать строку в нижний регистр
    static std::string toLower(const std::string& str) {
        std::string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }

    // Trim пробелы слева
    static std::string ltrim(const std::string& str) {
        size_t start = 0;
        while (start < str.length() && std::isspace(str[start])) {
            ++start;
        }
        return str.substr(start);
    }

    // Trim пробелы справа
    static std::string rtrim(const std::string& str) {
        size_t end = str.length();
        while (end > 0 && std::isspace(str[end - 1])) {
            --end;
        }
        return str.substr(0, end);
    }

    // Trim пробелы с обеих сторон
    static std::string trim(const std::string& str) {
        return rtrim(ltrim(str));
    }
};
