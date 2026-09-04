#include <cstdio>

#include "AnalyticsEvent.h"

namespace {
bool IsUnreserved(unsigned char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9')
            || ch == '-' || ch == '_' || ch == '.' || ch == '~';
}

std::string PercentEncode(const std::string &value) {
    std::string result;
    char encoded[4];
    for (const auto ch : value) {
        const auto byte = static_cast<unsigned char>(ch);
        if (IsUnreserved(byte)) {
            result.push_back(ch);
        } else {
            std::snprintf(encoded, sizeof(encoded), "%%%02X", byte);
            result += encoded;
        }
    }
    return result;
}

std::string JsonEscape(const std::string &value) {
    std::string result;
    char encoded[7];
    for (const auto ch : value) {
        switch (ch) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    std::snprintf(encoded, sizeof(encoded), "\\u%04X",
                                  static_cast<unsigned char>(ch));
                    result += encoded;
                } else {
                    result.push_back(ch);
                }
        }
    }
    return result;
}
}

std::string nsAnalyticsEvent::Format(
        const std::string &action,
        const std::map<std::string, std::string> &params) {
    auto result = PercentEncode(action);
    for (const auto &[name, value] : params) {
        result += ';';
        result += PercentEncode(name);
        result += '=';
        result += PercentEncode(value);
    }
    return result;
}

bool nsAnalyticsEvent::IsValid(const std::string &event) {
    return !event.empty() && event.size() <= MAX_ENCODED_LENGTH;
}

std::string nsAnalyticsEvent::BuildAppId(
        const std::string &baseAppId,
        const std::string &debugSuffix,
        bool debugBuild) {
    return debugBuild ? baseAppId + debugSuffix : baseAppId;
}

std::string nsAnalyticsEvent::BuildPayload(
        const std::string &appId,
        const std::string &location,
        const std::string &event) {
    return "{\"appId\":\"" + JsonEscape(appId)
            + "\",\"location\":\"" + JsonEscape(location)
            + "\",\"event\":\"" + JsonEscape(event) + "\"}";
}
