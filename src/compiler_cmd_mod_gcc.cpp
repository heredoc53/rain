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
using module::ModuleInfo;

namespace compiler::cmd::gcc::mod {
//clang++ -std=c++20 {} --precompile -fmodule-file={}={} -o {}
Command compile_object(const BuildFile& cfg, const ModuleInfo& mod, const vector<ModuleInfo>& mods){
    Command com{};

    const auto append_arg = [&args = com.args](const optional<string>& flag, const string& value){
        if(flag.has_value()){
            args.push_back(flag.value());
            args.push_back(value);
        }else{
            args.push_back(value);
        }
    };

    const auto find_pcm_by_name = [&mods](const string& modname) -> fs::path {
        for(const auto& m : mods)
            if(m.name == modname)
                return m.pcm;
        return fs::path();
    };

    com.name = cfg.cc;

    append_arg(std::nullopt, "-std=c++20");
    append_arg(std::nullopt, mod.file.string());
    append_arg(std::nullopt, "--precompile");

    for(const auto& import : mod.imports)
        append_arg(std::nullopt, fmt::format("-fmodule-file={}={}", import, find_pcm_by_name(import).string() ));

    append_arg("-o", mod.pcm.string());

    return com;
}

//clang++ -std=c++20 {} -fmodule-file={}={} {} -o {}
Command link_executable(const BuildFile& cfg, const vector<ModuleInfo>& mods){
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

    append_arg(std::nullopt, "-std=c++20");
    append_arg(std::nullopt, cfg.module_main.string());

    for(const auto& mod : mods)
        append_arg(std::nullopt, fmt::format("-fmodule-file={}={}", mod.name, mod.pcm.string()));

    for(const auto& mod : mods)
        append_arg(std::nullopt, mod.pcm.string());

    if constexpr(os == "windows")
        append_arg("-o", fmt::format("{}.exe", cfg.bin_path.string()));
    else if constexpr(os == "linux")
        append_arg("-o", cfg.bin_path.string());

    return com;
}

}
