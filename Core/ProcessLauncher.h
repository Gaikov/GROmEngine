// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <string>
#include <vector>

struct nsProcessLaunchInfo {
    std::string executable;
    std::vector<std::string> arguments;
    std::string workingDirectory;
};

class nsProcessLauncher final {
public:
    [[nodiscard]] static bool LaunchDetached( const nsProcessLaunchInfo &info );
};
