#pragma once

#include <iostream>
#include <optional>

namespace platform {

std::optional<std::string> get_environment_variable(const std::string& var);

constexpr std::string_view get_os(){
#ifdef _WIN32
    return "windows";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}

constexpr char get_path_separator(){
#if defined(_WIN32) || defined(_WIN64)
    return '\\';
#elif defined(__linux__)
    return '/';
#endif
}

constexpr char get_binpath_separator(){
#if defined(_WIN32) || defined(_WIN64)
    return ';';
#elif defined(__linux__)
    return ':';
#endif
}

}