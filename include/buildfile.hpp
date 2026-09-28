#pragma once

#include <toml++/toml.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <optional>

namespace build {

struct BuildTarget{
    std::vector<std::filesystem::path> sources;
    std::vector<std::filesystem::path> includes;
    std::vector<std::filesystem::path> library_paths;
    std::vector<std::string> libraries;
};

struct BuildFile {
    std::string name;
    std::string cc;
    std::filesystem::path bin_path;
    std::filesystem::path module_main; // when using modules there has to be one cpp file that has main function in it
    bool is_mod{false};
    const std::filesystem::path build_dir{"build"};
    BuildTarget main_target;
};

BuildFile parse_build_file();

}
