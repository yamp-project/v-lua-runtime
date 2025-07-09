#pragma once

#include <string>
#include <cctype>

#define MAX_STR_SIZE 64

namespace utils
{
    inline std::string StrToCamelCase(const char* input)
    {
        if (input == nullptr) {
            return {};
        }

        bool to_upper = false;
        std::string result;

        for (size_t i = 0; input[i] != '\0'; ++i) {
            if (i >= MAX_STR_SIZE) {
                break;
            }

            char ch = input[i];
            if (ch == '_') {
                to_upper = true;
                continue;
            }

            if (result.empty()) {
                result += std::tolower(static_cast<unsigned char>(ch));
            } else if (to_upper) {
                result += std::toupper(static_cast<unsigned char>(ch));
                to_upper = false;
            } else {
                result += std::tolower(static_cast<unsigned char>(ch));
            }
        }

        return result;
    }
}
