#include <compiler_cmd.hpp>
#include <platform.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>

//#define FMT_HEADER_ONLY
#include <fmt/core.h>

namespace fs = std::filesystem;
using std::vector;
using std::string;
using std::optional;
using build::BuildFile;
using command::Command;

namespace compiler::cmd::gcc {

Command compile_object(const BuildFile& cfg, const fs::path& source, const fs::path& object){
    Command com{};

    const auto append_arg = [&args = com.args](const string& flag, const string& value){
        args.push_back(flag);
        args.push_back(value);
    };

    com.name = cfg.cc;

    append_arg("-c", source.string());
    append_arg("-o", object.string());

    for(const auto& inc : cfg.main_target.includes)//header_files
        append_arg("-I", inc.string());

    return com;
}

Command link_executable(const BuildFile& cfg, const vector<fs::path>& objects){
    Command com{};
    constexpr auto os = platform::get_os();

    const auto append_arg = [&args = com.args](const optional<string>& flag, const string& value){
        if(flag.has_value()){
            args.push_back(flag.value());
            args.push_back(value);
        }else{
            args.push_back(value);
        }
    };
    // compiler is command name
    com.name = cfg.cc;
    //add all of object files
    for(const auto& object : objects)
        append_arg(std::nullopt, object.string());

    if constexpr(os == "windows")
        append_arg("-o", fmt::format("{}.exe", cfg.bin_path.string()));
    else if constexpr(os == "linux")
        append_arg("-o", cfg.bin_path.string());
    // insert library paths and library arguments into full command
    for(const auto& lpath : cfg.main_target.library_paths)//lib_paths
        append_arg("-L", lpath.string());

    for(const auto& lib : cfg.main_target.libraries)//libs
        append_arg("-l", lib);

    return com;
}

}
