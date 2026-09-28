#include <iostream>
#include <filesystem>
#include <stdexcept>

#include <compiler_cmd.hpp>
#include <compiler.hpp>
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

CompilerClangModules::CompilerClangModules(BuildFile bfile) : Compiler(bfile) {
    if(bfile.main_target.sources.empty())
        throw std::runtime_error("No source files specified");

    if(const auto [installed, _] = file::is_installed("clang++"); !installed)
        throw std::runtime_error("Compiler clang++ is not installed");

    if(!file::does_exists(bfile.module_main))
        throw std::runtime_error("Source file with main function for modules is not defined\nplease set option \"module_main\"");

    if(!file::does_exists(bfile.build_dir))
        fs::create_directory(bfile.build_dir);
}

// TODO: add checks if module exists as it can capture imports for files that doesnt exist
void CompilerClangModules::compile_to_objects(){
    // parse information like name of module from export keyword, its imports and file where it located
    for(const auto& mod : bfile.main_target.sources)
        modules.push_back(module::parse_module(mod));

    modules = module::topology_sort(modules);
    // create paths for pcm files
    for(auto& mod : modules)
        mod.pcm = file::source_to_object(mod.file, bfile.build_dir, ".pcm");

    for(const auto& mod : modules){
        const auto com = cmd::gcc::mod::compile_object(bfile, mod, modules);
        const auto output = command::execute_command(com);

        print_compiler_output(output);
    }

}

void CompilerClangModules::link_to_executable(){
    const auto com = cmd::gcc::mod::link_executable(bfile, modules);
    const auto output = command::execute_command(com);

    print_compiler_output(output);
}

}
