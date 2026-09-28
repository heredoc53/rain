#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <string_view>

#include <platform.hpp>
//fs::path can implicitly convert to string
namespace file {

std::filesystem::path source_to_object(const std::filesystem::path& src_name, const std::filesystem::path& dir, std::string ext);
void list_files(const std::filesystem::path& dir, const std::string& ext, std::vector<std::filesystem::path>& files);
std::filesystem::path resolve_file_path(std::string path);

bool does_exists(std::string_view entry);
bool does_exists_dir(std::string_view entry);
bool does_exists(const std::filesystem::path& entry);
bool does_exists_dir(const std::filesystem::path& entry);

std::tuple<bool, std::filesystem::path> is_installed(std::string_view name);
bool needs_recompile(const std::filesystem::path& source, const std::filesystem::path& object);

}
