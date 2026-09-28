#include <iostream>
#include <filesystem>

#include <compiler.hpp>
#include <compiler_cmd.hpp>
#include <file.hpp>
#include <platform.hpp>
#include <command.hpp>

//#define FMT_HEADER_ONLY
#include <fmt/core.h>

namespace fs = std::filesystem;
using std::string;
using std::optional;
using build::BuildFile;

namespace compiler {

CompilerMSVCStyle::CompilerMSVCStyle(BuildFile bfile) : Compiler(bfile) {
    if(bfile.main_target.sources.empty())
        throw std::runtime_error("No source files specified");

    if(const auto [installed, _] = file::is_installed(bfile.cc); !installed)
        throw std::runtime_error(fmt::format("Compiler {} is not installed", bfile.cc));

    if(!file::does_exists(bfile.build_dir))
        fs::create_directory(bfile.build_dir);
}

void CompilerMSVCStyle::compile_to_objects() {
    for(const auto& source : bfile.main_target.sources){
        const auto object = file::source_to_object(source, bfile.build_dir, ".obj");
        objects.push_back(object);

        if(!file::needs_recompile(source, object)){
            fmt::println("File {} wasnt modified, so it doesnt need to be recompiled", source.string());
            continue;
        }

        const auto com = cmd::msvc::compile_object(bfile, source, object);
        const auto output = command::execute_command(com);

        print_compiler_output(output);
    }
}

void CompilerMSVCStyle::link_to_executable() {
    const auto com = cmd::msvc::link_executable(bfile, objects);
    const auto output = command::execute_command(com);

    print_compiler_output(output);
}

}
