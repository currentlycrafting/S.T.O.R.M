#include "store.h"
#include <iostream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <cctype>

/**
 * @brief Trim leading and trailing whitespace from a string.
 * @param inputString String to trim.
 * @return std::string Trimmed string.
 */
std::string TrimWhitespace(const std::string& inputString) {
    std::string trimmedString = inputString;

    // Trim leading whitespace
    trimmedString.erase(
        trimmedString.begin(),
        std::find_if(
            trimmedString.begin(),
            trimmedString.end(),
            [](unsigned char character) { return !std::isspace(character); }
        )
    );

    // Trim trailing whitespace
    trimmedString.erase(
        std::find_if(
            trimmedString.rbegin(),
            trimmedString.rend(),
            [](unsigned char character) { return !std::isspace(character); }
        ).base(),
        trimmedString.end()
    );

    return trimmedString;
}

/**
 * @brief Main entry point for the interactive Store CLI.
 *
 * Provides commands:
 *  - PUT key value    : Insert or update a key-value pair
 *  - GET key          : Retrieve the value for a key
 *  - DEL key          : Delete a key
 *  - LIST             : Display all keys and values
 *  - CLEAR            : Remove all keys
 *  - HELP             : Show available commands
 *  - HISTORY          : Show recent commands
 *  - EXIT             : Exit CLI
 */
int main() {
    // Create the in-memory key-value store
    Store keyValueStore;

    // Command history buffer
    std::deque<std::string> commandHistory;
    const size_t kMaximumHistorySize = 50;

    std::cout << "Store CLI started. Commands: PUT, GET, DEL, LIST, CLEAR, HELP, HISTORY, EXIT\n";

    std::string userInputLine;

    while (true) {
        std::cout << "> ";

        // Read user input line
        if (!std::getline(std::cin, userInputLine)) {
            break; // Exit on EOF
        }

        // Trim whitespace from user input
        std::string trimmedInputLine = TrimWhitespace(userInputLine);

        // Save command in history if not empty
        if (!trimmedInputLine.empty()) {
            commandHistory.push_back(trimmedInputLine);
            if (commandHistory.size() > kMaximumHistorySize) {
                commandHistory.pop_front();
            }
        }

        // Parse input into command keyword and arguments
        std::istringstream inputStream(trimmedInputLine);
        std::string commandKeyword;
        inputStream >> commandKeyword;

        // ===========================
        // Command: PUT
        // ===========================
        if (commandKeyword == "PUT") {
            std::string keyArgument;
            std::string valueArgument;
            inputStream >> keyArgument >> valueArgument;

            if (keyArgument.empty() || valueArgument.empty()) {
                std::cout << "{ \"success\": false, \"error\": \"PUT requires key and value\" }\n";
                continue;
            }

            keyValueStore.put(keyArgument, valueArgument);
            std::cout << "{ \"success\": true }\n";
        }
        // ===========================
        // Command: GET
        // ===========================
        else if (commandKeyword == "GET") {
            std::string keyArgument;
            inputStream >> keyArgument;

            if (keyArgument.empty()) {
                std::cout << "{ \"success\": false, \"error\": \"GET requires key\" }\n";
                continue;
            }

            std::string retrievedValue;
            if (keyValueStore.get(keyArgument, retrievedValue)) {
                std::cout << "{ \"success\": true, \"value\": \"" << retrievedValue << "\" }\n";
            } else {
                std::cout << "{ \"success\": false, \"error\": \"Key not found\" }\n";
            }
        }
        // ===========================
        // Command: DEL
        // ===========================
        else if (commandKeyword == "DEL") {
            std::string keyArgument;
            inputStream >> keyArgument;

            if (keyArgument.empty()) {
                std::cout << "{ \"success\": false, \"error\": \"DEL requires key\" }\n";
                continue;
            }

            if (keyValueStore.del(keyArgument)) {
                std::cout << "{ \"success\": true }\n";
            } else {
                std::cout << "{ \"success\": false, \"error\": \"Key not found\" }\n";
            }
        }
        // ===========================
        // Command: LIST
        // ===========================
        else if (commandKeyword == "LIST") {
            keyValueStore.list();
        }
        // ===========================
        // Command: CLEAR
        // ===========================
        else if (commandKeyword == "CLEAR") {
            keyValueStore.clear();
            std::cout << "{ \"success\": true }\n";
        }
        // ===========================
        // Command: HELP
        // ===========================
        else if (commandKeyword == "HELP") {
            std::cout << "Commands:\n";
            std::cout << "  PUT key value    - store key with value\n";
            std::cout << "  GET key          - retrieve value for key\n";
            std::cout << "  DEL key          - delete key\n";
            std::cout << "  LIST             - list all keys (most recent first)\n";
            std::cout << "  CLEAR            - remove all keys\n";
            std::cout << "  HISTORY          - show recent commands\n";
            std::cout << "  HELP             - show this message\n";
            std::cout << "  EXIT             - quit\n";
        }
        // ===========================
        // Command: HISTORY
        // ===========================
        else if (commandKeyword == "HISTORY") {
            std::cout << "{ \"history\": [\n";
            for (size_t index = 0; index < commandHistory.size(); ++index) {
                std::cout << "  \"" << commandHistory[index] << "\"";
                if (index + 1 < commandHistory.size()) {
                    std::cout << ",";
                }
                std::cout << "\n";
            }
            std::cout << "] }\n";
        }
        // ===========================
        // Command: EXIT
        // ===========================
        else if (commandKeyword == "EXIT") {
            break;
        }
        // ===========================
        // Unknown command
        // ===========================
        else if (!commandKeyword.empty()) {
            std::cout << "{ \"error\": \"Unknown command\" }\n";
        }
    }

    return 0;
}
