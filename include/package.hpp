#pragma once

#include <iostream>
#include <filesystem>
#include <toml++/toml.hpp>

namespace package{

struct PackageData{
    std::vector<std::string> libpath;
    std::vector<std::string> headers;
    std::vector<std::string> libs;
};

PackageData parse_package(const std::string& file_name);
void print_package(const std::string& file_name);
void create_package(const std::string& pkg_name, const PackageData& pkg);

}
