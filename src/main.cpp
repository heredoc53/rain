#include <iostream>
#include <filesystem>
#include <vector>
#include <string_view>

#include <buildfile.hpp>
#include <compiler.hpp>
#include <package.hpp>
#include <command.hpp>

#include <toml++/toml.hpp>
#include <argparse/argparse.hpp>
//#define FMT_HEADER_ONLY
#include <fmt/core.h>

using std::string;
using std::vector;
using build::BuildFile;
using package::PackageData;
using argparse::ArgumentParser;

//this function is used in main to print out message and return 1 right away
[[nodiscard("This function should be used with return")]]
int exit_with_message (std::string_view message, auto... args){
    fmt::println(fmt::runtime(message), args...);
    return 1;
}

void build_command(){
    const BuildFile build_file = build::parse_build_file();
    compiler::run_compiler(build_file);
}

void print_package_command(const ArgumentParser& print_cmd){
    const string name = print_cmd.get<string>("name");
    package::print_package(name);
}

void add_package_command(const ArgumentParser& add_cmd){
    const string name = add_cmd.get<string>("name");
    const PackageData pkg {
        .libpath = add_cmd.get<vector<string>>("-L"),
        .headers = add_cmd.get<vector<string>>("-I"),
        .libs = add_cmd.get<vector<string>>("-l"),
    };
    package::create_package(name, pkg);
}


int main(int argc, char* argv[]){

    ArgumentParser program{"rain"};
    ArgumentParser build_cmd{"build"};
    ArgumentParser package_cmd{"package"};

    ArgumentParser print_cmd{"print"};
    print_cmd.add_argument("name");

    ArgumentParser add_cmd{"add"};
    add_cmd.add_argument("name");
    add_cmd.add_argument("-L")
        .nargs(argparse::nargs_pattern::any)
        .default_value(vector<string>{})
        .help("Add library paths");
    add_cmd.add_argument("-I")
        .nargs(argparse::nargs_pattern::any)
        .default_value(vector<string>{})
        .help("Add Include paths");
    add_cmd.add_argument("-l")
        .nargs(argparse::nargs_pattern::any)
        .default_value(vector<string>{})
        .help("Add libraries to be linked");

    package_cmd.add_subparser(print_cmd);
    package_cmd.add_subparser(add_cmd);

    program.add_subparser(build_cmd);
    program.add_subparser(package_cmd);

    //Note: this is not big problem, but error catching should be improved
    //it cathes every exception, but there is no way to tell usually exactly what error it is
    //it just prints error message that usually not informative,there is no handling for specific types of exceptions
    try{
        program.parse_args(argc, argv);
        // it uses package::parse_package, toml errors will be caught in build file catch block
        if(program.is_subcommand_used("build")){
            build_command(); //toml error catch block for packages wont caught it
        }else if(program.is_subcommand_used("package")){
            try{ //this handles toml errors that can happend with packages
                if(package_cmd.is_subcommand_used("print"))
                    print_package_command(print_cmd);
                else if(package_cmd.is_subcommand_used("add"))
                    add_package_command(add_cmd);
                else
                    return exit_with_message(program.help().str());
            }catch(const toml::parse_error& ex){
                return exit_with_message("Error working with packages:\n{}\nPlease check package files for errors", ex.what());
            }
        }else{
            return exit_with_message(program.help().str());
        }
    }catch(const toml::parse_error& ex){ //this handles toml errors during build file parsing
        return exit_with_message("Error parsing build file:\n{}\nPlease check build file for syntax errors", ex.what());
    }catch(const std::exception& ex){
        return exit_with_message("Error: {}\n{}", ex.what(), program.help().str());
    }

    return 0;
}
