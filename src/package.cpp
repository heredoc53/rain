#include <package.hpp>
#include <platform.hpp>
#include <toml_util.hpp>

#include <optional>
#include <filesystem>
#include <stdexcept>
#include <fstream>

//#define FMT_HEADER_ONLY
#include <fmt/core.h>

namespace fs = std::filesystem;
using std::string;
using std::vector;
using std::optional;

namespace {

struct PackageFile {
    toml::table file;
    fs::path file_path;
};
// if dont_parse is set to true then it should only set file_path for cases like create_package
// where toml::table file should be build manually and worked with
PackageFile get_package_file(const string& file_name, bool should_parse = true){
    PackageFile pkg_file;
    const auto pkg_path = platform::get_environment_variable("RAIN_PACKAGE_PATH");

    if(!pkg_path.has_value()){
        throw std::runtime_error("Environment variable RAIN_PACKAGE_PATH was not set, please define it");
    }

    pkg_file.file_path = fs::path(pkg_path.value()) / file_name;
    pkg_file.file_path.replace_extension(".toml");

    if(should_parse ==  true){
        pkg_file.file = toml::parse_file(pkg_file.file_path.string());
    }

    return pkg_file;
}

}

namespace package {

PackageData parse_package(const string& file_name){
    const PackageFile pkg_file = get_package_file(file_name);
    PackageData pkg_data;

    // we opened package file, now we need to parse each element to package data, and return it
    const auto parse_option = [&pkg_file](const string& option_name, vector<string>& pkg_data){
        const auto option = pkg_file.file[option_name];
        const auto array = toml_util::toml_array_to_vec(option);

        for(const auto& elem : array){
            pkg_data.push_back(elem);
        }
    };

    parse_option("library_paths", pkg_data.libpath);
    parse_option("includes", pkg_data.headers);
    parse_option("libraries", pkg_data.libs);

    return pkg_data;
}

void print_package(const string& file_name){
    const PackageData pkg = parse_package(file_name);

    const auto read_option = [](const string& option_name, const vector<string>& option){
        fmt::println("{}:", option_name);
        for(const auto& elem : option){
            fmt::println("---{}", elem);
        }
    };

    read_option("Includes", pkg.headers);
    read_option("Library paths", pkg.libpath);
    read_option("Libraries", pkg.libs);
}

void create_package(const string& pkg_name, const PackageData& pkg){
    PackageFile pkg_file = get_package_file(pkg_name, false);

    const auto to_toml_arr = [](const vector<string>& vec) -> toml::array {
        toml::array arr;
        for(const auto& elem : vec)
            arr.push_back(elem);
        return arr;
    };

    pkg_file.file.insert("library_paths", to_toml_arr(pkg.libpath));
    pkg_file.file.insert("includes", to_toml_arr(pkg.headers));
    pkg_file.file.insert("libraries", to_toml_arr(pkg.libs));

    std::ofstream file(pkg_file.file_path.string());
    file << pkg_file.file;
}

}
