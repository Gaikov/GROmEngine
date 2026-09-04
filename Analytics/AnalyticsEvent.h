#pragma once

#include <cstddef>
#include <map>
#include <string>

namespace nsAnalyticsEvent {
constexpr size_t MAX_ENCODED_LENGTH = 128;

std::string Format(
        const std::string &action,
        const std::map<std::string, std::string> &params = {});
bool IsValid(const std::string &event);
std::string BuildAppId(
        const std::string &baseAppId,
        const std::string &debugSuffix,
        bool debugBuild);
std::string BuildPayload(
        const std::string &appId,
        const std::string &location,
        const std::string &event);
}
