#include <stdexcept>
#include <memory>
#include <iostream>
#include <array>
#include <string_view>

#include <compiler.hpp>
#include <platform.hpp>
#include <file.hpp>
#include <fmt/core.h>

using std::tuple;
using std::array;
using std::string;
using std::optional;
using std::unique_ptr;
using std::string_view;
using build::BuildFile;
using compiler::Compiler;
using compiler::CompilerGCCStyle;
using compiler::CompilerClangModules;
using compiler::CompilerMSVCStyle;

using CompilerOutput = tuple<string, string, int>;

namespace {

unique_ptr<Compiler> get_compiler(const BuildFile& bfile){
    if((bfile.cc == "g++" || bfile.cc == "clang++") && !bfile.is_mod)
        return std::make_unique<CompilerGCCStyle>(bfile);
    else if(bfile.cc == "clang++" && bfile.is_mod)
        return std::make_unique<CompilerClangModules>(bfile);
    else if(bfile.cc == "cl" || bfile.cc == "clang-cl")
        return std::make_unique<CompilerMSVCStyle>(bfile);
    else
        return nullptr;
}

optional<string> find_default_compiler_unix(){
    constexpr array<string_view, 2> default_cc = {"g++", "clang++"};

    for(const auto& cc : default_cc)
        if(const auto [installed, bin] = file::is_installed(cc); installed)
            return bin;

    return std::nullopt;
}

}

namespace compiler {

Compiler::Compiler(BuildFile bfile) : bfile(bfile){ }

void run_compiler(const BuildFile& bfile){
    auto compiler = get_compiler(bfile);

    if(!compiler){
        throw std::runtime_error(fmt::format("Unknown compiler: {}", bfile.cc));
    }
    //in case of error it will throw 
    compiler->compile_to_objects();
    compiler->link_to_executable();
}

optional<string> find_default_compiler(){
    constexpr auto os = platform::get_os();

    if constexpr(os == "linux")
        return find_default_compiler_unix();
    else if constexpr(os == "windows")
        throw std::runtime_error("Finding default compiler for windows is currently not implemented");

    return std::nullopt;
}

void print_compiler_output(const CompilerOutput& output){
    const auto [out, err, ret_code] = output;

    fmt::println("Compiler exited with: {}", ret_code);
    if(!out.empty())
        fmt::println("Compiler output: {}", out);
    else if(!err.empty())
        fmt::println("Compiler error: {}", err);
}

}
