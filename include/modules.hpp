#pragma once

#include <iostream>
#include <vector>
#include <filesystem>

namespace module {

struct ModuleInfo{
    std::string name;
    std::filesystem::path file;
    std::filesystem::path pcm;
    std::vector<std::string> imports;
};

ModuleInfo parse_module(const std::filesystem::path& file_name);
std::vector<ModuleInfo> topology_sort(const std::vector<ModuleInfo>& mods);

}