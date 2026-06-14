#include "Json.h"

std::string ParseJsonString(const std::string& body, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = body.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = body.find(":", keyPos);
    if (colonPos == std::string::npos) return "";

    size_t quoteStart = body.find("\"", colonPos);
    if (quoteStart == std::string::npos) return "";

    size_t quoteEnd = body.find("\"", quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";

    return body.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

std::string ParseJsonValueRaw(const std::string& body, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = body.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = body.find(":", keyPos + searchKey.length());
    if (colonPos == std::string::npos) return "";

    // 跳过空格
    size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
    if (valueStart == std::string::npos) return "";

    // 判断值类型
    if (body[valueStart] == '"') {
        // 字符串值
        size_t valueEnd = body.find("\"", valueStart + 1);
        if (valueEnd == std::string::npos) return "";
        return body.substr(valueStart + 1, valueEnd - valueStart - 1);
    } else {
        // 数字或布尔值 — 找到逗号、花括号闭合或换行
        size_t valueEnd = body.find_first_of(",}\n\r", valueStart);
        if (valueEnd == std::string::npos) valueEnd = body.length();
        std::string raw = body.substr(valueStart, valueEnd - valueStart);
        // 去掉尾部空格
        while (!raw.empty() && (raw.back() == ' ' || raw.back() == '\t')) {
            raw.pop_back();
        }
        return raw;
    }
}