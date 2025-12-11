// Copyright
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include "../utils/StringUtils.hpp"
#include "Database.hpp"

struct CommandResult {
    bool success;
    std::string output;
    std::string error;
};

class CommandParser {
 public:
    explicit CommandParser(Database& db) : db(db) {}

    CommandResult execute(const std::string& query) {
        std::vector<std::string> tokens = StringUtils::split(query);

        if (tokens.empty()) {
            return {false, "", "Empty query"};
        }

        std::string command = StringUtils::toLower(tokens[0]);

        try {
            // SET операции
            if (command == "sadd") {
                return parseSADD(tokens);
            } else if (command == "srem") {
                return parseSREM(tokens);
            } else if (command == "sismember") {
                return parseSISMEMBER(tokens);
            }
            // STACK операции
            else if (command == "spush") {
                return parseSPUSH(tokens);
            } else if (command == "spop") {
                return parseSPOP(tokens);
            }
            // QUEUE операции
            else if (command == "qpush") {
                return parseQPUSH(tokens);
            } else if (command == "qpop") {
                return parseQPOP(tokens);
            }
            // HASH операции
            else if (command == "hset") {
                return parseHSET(tokens);
            } else if (command == "hdel") {
                return parseHDEL(tokens);
            } else if (command == "hget") {
                return parseHGET(tokens);
            }
            // неизвестная команда
            else {
                return {false, "", "Unknown command: " + command};
            }
        } catch (const std::exception& e) {
            return {false, "", e.what()};
        }
    }

 private:
    Database& db;

    // SET ОПЕРАЦИИ 
    CommandResult parseSADD(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "SADD requires: setName value"};
        }

        std::string setName = tokens[1];
        std::string value = tokens[2];

        db.setAdd(setName, value);
        return {true, value, ""};
    }

    CommandResult parseSREM(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "SREM requires: setName value"};
        }

        std::string setName = tokens[1];
        std::string value = tokens[2];

        db.setRem(setName, value);
        return {true, value, ""};
    }

    CommandResult parseSISMEMBER(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "SISMEMBER requires: setName value"};
        }

        std::string setName = tokens[1];
        std::string value = tokens[2];

        bool result = db.setIsMember(setName, value);
        return {true, result ? "TRUE" : "FALSE", ""};
    }

    // STACK ОПЕРАЦИИ
    CommandResult parseSPUSH(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "SPUSH requires: stackName value"};
        }

        std::string stackName = tokens[1];
        std::string value = tokens[2];

        db.stackPush(stackName, value);
        return {true, value, ""};
    }

    CommandResult parseSPOP(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            return {false, "", "SPOP requires: stackName"};
        }

        std::string stackName = tokens[1];
        std::string value = db.stackPop(stackName);
        return {true, value, ""};
    }

    // QUEUE ОПЕРАЦИИ
    CommandResult parseQPUSH(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "QPUSH requires: queueName value"};
        }

        std::string queueName = tokens[1];
        std::string value = tokens[2];

        db.queuePush(queueName, value);
        return {true, value, ""};
    }

    CommandResult parseQPOP(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            return {false, "", "QPOP requires: queueName"};
        }

        std::string queueName = tokens[1];
        std::string value = db.queuePop(queueName);
        return {true, value, ""};
    }

    // HASH ОПЕРАЦИИ
    CommandResult parseHSET(const std::vector<std::string>& tokens) {
        if (tokens.size() < 4) {
            return {false, "", "HSET requires: hashName key value"};
        }

        std::string hashName = tokens[1];
        std::string key = tokens[2];
        std::string value = tokens[3];

        db.hashSet(hashName, key, value);
        return {true, value, ""};
    }

    CommandResult parseHDEL(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "HDEL requires: hashName key"};
        }

        std::string hashName = tokens[1];
        std::string key = tokens[2];

        db.hashDel(hashName, key);
        return {true, key, ""};
    }

    CommandResult parseHGET(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            return {false, "", "HGET requires: hashName key"};
        }

        std::string hashName = tokens[1];
        std::string key = tokens[2];

        std::string value = db.hashGet(hashName, key);
        return {true, value.empty() ? "(nil)" : value, ""};
    }
};
