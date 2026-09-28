#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <ranges>
#include <algorithm>
#include <string_view>

#include <file.hpp>
#include <platform.hpp>

//#define FMT_HEADER_ONLY
#include <fmt/core.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace fs = std::filesystem;
namespace vws = std::views;

using std::vector;
using std::string;
using std::tuple;
using std::string_view;

namespace file {

fs::path resolve_file_path(string path){
    if(path.rfind("f:", 0) == 0) // use full path, remove first two symbols and return path
        return fs::path(path.substr(2));
    //use path with dots as path separator, if it has other symbols as separator, its an error
    if(path.contains('\\') || path.contains('/'))
        throw std::runtime_error(fmt::format("Path {} contains wrong symbols", path));
    // iterate through string, and apply lambda for each symbol
    constexpr char from = '.', to = platform::get_path_separator();
    std::transform(path.begin(), path.end(), path.begin(), [&](char c) {
        return c == from ? to : c; //if its dot, replace current char else return that char back, dont change anything
    });

    return fs::path(path);
}

void list_files(const fs::path& dir, const string& ext, vector<fs::path>& files){
    for(const auto& file : fs::directory_iterator(dir)){
        if(file.path().extension() == ext){
            files.push_back(file);
        }
    }
}

fs::path source_to_object(const fs::path& src_name, const fs::path& dir, string ext){
    fs::path file = src_name.filename().replace_extension(ext);
    fs::path output = dir / file;
    return output;
}

bool does_exists(string_view entry){
    return fs::exists(fs::path(entry)) && fs::is_regular_file(fs::path(entry));
}

bool does_exists_dir(string_view entry){
    return fs::exists(fs::path(entry)) && fs::is_directory(fs::path(entry));
}

bool does_exists(const fs::path& entry){
    return fs::exists(entry) && fs::is_regular_file(entry);
}

bool does_exists_dir(const fs::path& entry){
    return fs::exists(entry) && fs::is_directory(entry);
}

[[nodiscard]]
bool is_executable(const fs::path& path){
    if(!does_exists(path) || fs::is_directory(path))
        return false;
#ifdef _WIN32
    DWORD bin_type;
    return GetBinaryTypeA(path.string().c_str(), &bin_type);
#elif defined(__linux__)
    return access(path.string().c_str(), X_OK) == 0;
#endif
}

tuple<bool, fs::path> is_installed(string_view name){
    vector<fs::path> paths;
    constexpr auto bin_sep = platform::get_binpath_separator();
    constexpr auto os = platform::get_os();
    //paths = std::views::split(path, platform::get_binpath_separator()) | std::ranges::to<vector>();
    if(const auto res = platform::get_environment_variable("PATH"); res.has_value())
        for(auto&& p : res.value() | vws::split(bin_sep) | vws::transform([](const auto&& pr){
                return fs::path(string(pr.begin(), pr.end())); }))
            paths.push_back(std::move(p));//emplace_back(p.begin(), p.end());
    else
        throw std::runtime_error("Couldnt find PATH variable");

    for(fs::path path : paths){
        if constexpr(os == "windows")
            path = path / (string{name} + ".exe");
        else if constexpr(os == "linux")
            path = path / name;

        if(does_exists(path) && is_executable(path))
            return std::make_tuple(true, path);
    }
    return std::make_tuple(false, fs::path());
}

bool needs_recompile(const fs::path& source, const fs::path& object){
    if(!does_exists(object)) //if object file doesnt exist, then its should be compiled
        return true;
    //source_time > object_time
    return fs::last_write_time(source) > fs::last_write_time(object) ? true : false;
}

}
