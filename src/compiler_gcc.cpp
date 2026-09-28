#include <iostream>
#include <filesystem>
#include <stdexcept>

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

CompilerGCCStyle::CompilerGCCStyle(BuildFile bfile) : Compiler(bfile) {
    if(bfile.main_target.sources.empty())
        throw std::runtime_error("No source files specified");

    if(const auto [installed, _] = file::is_installed(bfile.cc); !installed)
        throw std::runtime_error(fmt::format("Compiler {} is not installed", bfile.cc));

    if(!file::does_exists(bfile.build_dir))
        fs::create_directory(bfile.build_dir);
}

void CompilerGCCStyle::compile_to_objects() {
    for(const auto& source : bfile.main_target.sources){
        const auto object = file::source_to_object(source, bfile.build_dir, ".o");
        objects.push_back(object);

        if(!file::needs_recompile(source, object)){
            fmt::println("File {} wasnt modified, so it doesnt need to be recompiled", source.string());
            continue;
        }

        const auto com = cmd::gcc::compile_object(bfile, source, object);
        const auto output = command::execute_command(com);

        print_compiler_output(output);
    }
}

void CompilerGCCStyle::link_to_executable() {
    const auto com = cmd::gcc::link_executable(bfile, objects);
    const auto output = command::execute_command(com);

    print_compiler_output(output);
}

}

