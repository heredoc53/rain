#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <optional>

#include <file.hpp>
#include <buildfile.hpp>
#include <platform.hpp>
#include <compiler.hpp>
#include <toml_util.hpp>
#include <package.hpp>

#include <toml++/toml.hpp>
//#define FMT_HEADER_ONLY
#include <fmt/core.h>

namespace fs = std::filesystem;

using std::string;
using std::vector;
using std::optional;
using std::invocable;
using std::string_view;
using build::BuildFile;
using package::PackageData;

namespace {

constexpr string_view buildfile_name = "rain.toml";

struct TomlEntry {
    string name;
    toml::table content;
};

void parse_sources(const TomlEntry& entry, BuildFile& bfile){
    const auto src = entry.content["sources"];

    if(entry.name != "build" && !src){
        fmt::println("No sources specified for os specific target, skipping");
        return;
    }

    if(const auto is_mod = entry.content["is_modules"].value<bool>(); is_mod.has_value())
        bfile.is_mod = is_mod.value();

    const string ext = bfile.is_mod ? ".cppm" : ".cpp";

    if(const auto mod_main = entry.content["module_main"].value<string>(); bfile.is_mod && mod_main.has_value())
        bfile.module_main = mod_main.value();

    const auto option = toml_util::toml_array_to_vec(src);
    // Important, this doesnt check if two equal files was added
    for(const auto& elem : option){
        const auto full_path = file::resolve_file_path(elem);

        if(!file::does_exists_dir(full_path))
            throw std::runtime_error(fmt::format("Couldnt find source directory: {}", full_path.string()));

        file::list_files(full_path, ext, bfile.main_target.sources);
    }

}
// this function parse options like include and library_paths
// both options are collection of directories so they basically have same logic of parsing
void parse_directories(const TomlEntry& entry, vector<fs::path>& cfg_option, const string& field_name){
    const auto dirs = entry.content[field_name];

    if(!dirs){
        if(field_name == "includes")
            fmt::println("Includes (-I) are not specified, skipping");
        else if(field_name == "library_paths")
            fmt::println("Library paths (-L) are not specified, skipping");
        return;
    }

    const auto option = toml_util::toml_array_to_vec(dirs);

    for(const auto& elem : option){
        const auto full_path = file::resolve_file_path(elem);

        if(!file::does_exists_dir(full_path))
            throw std::runtime_error(fmt::format("Couldnt find include directories: {}", full_path.string()));

        const auto found = std::find(cfg_option.begin(), cfg_option.end(), full_path);
        if(found == cfg_option.end()){ // if not found, or not already added
            cfg_option.push_back(full_path);
        }
    }

}

//this parse -l option, its basically collection of names of libraries
void parse_libraries(const TomlEntry& entry, BuildFile& bfile){
    const auto libs = entry.content["libraries"];

    if(!libs){
        fmt::println("Libraries (-l) are not specified, skipping");
        return;
    }

    const auto option = toml_util::toml_array_to_vec(libs);

    for(const auto& elem : option){
        const auto found = std::find(bfile.main_target.libraries.begin(), bfile.main_target.libraries.end(), elem);
        if(found == bfile.main_target.libraries.end()){ //found == end() means not found
            bfile.main_target.libraries.push_back(elem);
        }
    }

}

void parse_packages(const TomlEntry& entry, BuildFile& bfile){
    const auto libs = entry.content["packages"];
    const auto artif_path = platform::get_environment_variable("RAIN_ARTIFACT_PATH");

    const auto append_cfg = [&artif_path]<class T>(const vector<string>& entry, vector<T>& cfg){
        for(const auto& libs : entry){
            T temp;
            if constexpr(std::is_same_v<T, fs::path>)
                temp = fs::path(artif_path.value()) / file::resolve_file_path(libs);
            else
                temp = libs;
            const auto found = std::find(cfg.begin(), cfg.end(), temp);
            if(found == cfg.end()){ //found == end() means not found
                cfg.push_back(temp);
            }
        }
    };

    if(!libs){
        fmt::println("Packages are not specified, skipping");
        return;
    }

    if(!artif_path.has_value())
        throw std::runtime_error("Couldnt find artifacts directory for packages");

    const auto option = toml_util::toml_array_to_vec(libs);

    for(const auto& elem : option){
        const PackageData pkg = package::parse_package(elem);
        append_cfg(pkg.libpath, bfile.main_target.library_paths);
        append_cfg(pkg.headers, bfile.main_target.includes);
        append_cfg(pkg.libs, bfile.main_target.libraries);
    }

}

void parse_required_options(const TomlEntry& target, BuildFile& bfile){
    //quite verbose but much better and safer than using macros
    const auto parse_value = [&target](string& bfile_value, string_view opt_name, invocable auto error_hanle){
        if(const auto result = target.content[opt_name].value<string>(); result.has_value())
            bfile_value = result.value();
        else
            error_hanle();
    };
    // parse required options that should be defined in [build] target
    if(target.name != "build")
        return;
    //parse name of program
    parse_value(bfile.name, "name", [](){
        throw std::runtime_error("Program name was not specified");
    });

    bfile.bin_path = bfile.build_dir / bfile.name;
    //parse compiler name, later it should be modified to automatically find compiler if not set
    parse_value(bfile.cc, "cc", [&cc = bfile.cc](){
        if(const auto def_cc = compiler::find_default_compiler(); def_cc.has_value())
            cc = def_cc.value();
        else
            throw std::runtime_error("Couldnt find any compiler");
    });
}


void parse_target(const TomlEntry& entry, BuildFile& bfile){
    parse_required_options(entry, bfile);
    parse_sources(entry, bfile);
    parse_directories(entry, bfile.main_target.includes, "includes");
    parse_directories(entry, bfile.main_target.library_paths, "library_paths");
    parse_libraries(entry, bfile);
    parse_packages(entry, bfile);
}

}

namespace build {

BuildFile parse_build_file(){
    BuildFile bfile;
    toml::table toml_file;// root table in file

    const auto run_parse = [&](const string& entry_name) {
        TomlEntry entry {
            .name{entry_name},// gcc cant accept this initialization if entry_name is string view, clang can accept it
            .content = toml_util::copy_toml_table(toml_file, entry.name)
        };
        fmt::println("Running target: {}", entry.name);
        parse_target(entry, bfile);
    };

    if(!file::does_exists(buildfile_name))
        throw std::runtime_error("Couldnt find build file, exiting");
    //parse_file might throw either because of syntax error in toml file
    //or because there is error in writing file paths on for windows, which is syntax error too basically
    toml_file = toml::parse_file(buildfile_name);
    run_parse("build");

    constexpr auto os_table = platform::get_os();
    if(toml_file.contains(os_table) && toml_file[os_table].is_table()){
        run_parse(string{os_table});
    }

    return bfile;
}

}
