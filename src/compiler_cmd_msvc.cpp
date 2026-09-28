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

//Compilation: cc /c {/I} /Fo:file.obj file.cpp
//Linking: cc *.obj /link {/LIBPATH:path} {*.lib} /OUT:name.exe
//instead of writing -l it requires to write library full name + .lib

namespace compiler::cmd::msvc {

Command compile_object(const BuildFile& cfg, const fs::path& source, const fs::path& object){
    Command com{};

    const auto append_arg = [&args = com.args](const optional<string>& flag, const string& value){
        if(flag.has_value()){
            args.push_back(flag.value());
            args.push_back(value);
        }else{
            args.push_back(value);
        }
    };

    com.name = cfg.cc;

    append_arg(std::nullopt, "/c");

    for(const auto& inc : cfg.main_target.includes)//header_files
        append_arg("-I", inc.string());

    append_arg(std::nullopt, fmt::format("/Fo:{}",object.string()));
    append_arg(std::nullopt, source.string());

    return com;
}

//Linking: cc *.obj /link {/LIBPATH:path} {*.lib} /OUT:name.exe
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
    //add this specific flag
    append_arg(std::nullopt, "/link");
    // insert library paths
    for(const auto& lpath : cfg.main_target.library_paths)//lib_paths
        append_arg(std::nullopt, fmt::format("/LIBPATH:{}", lpath.string()));
    //insert library arguments
    for(const auto& lib : cfg.main_target.libraries)//libs
        append_arg(std::nullopt, lib);

    if constexpr(os == "windows")
        append_arg(std::nullopt, fmt::format("/OUT:{}.exe", cfg.bin_path.string()));
    else if constexpr(os == "linux")
        append_arg(std::nullopt, fmt::format("/OUT:{}", cfg.bin_path.string()));

    return com;
}

}
