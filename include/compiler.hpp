#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <tuple>
#include <optional>

#include <buildfile.hpp>
#include <modules.hpp>

namespace compiler {

void run_compiler(const build::BuildFile& cfg);
void print_compiler_output(const std::tuple<std::string, std::string, int>&);
std::optional<std::string> find_default_compiler();

class Compiler{
protected:
    build::BuildFile bfile;
    std::vector<std::filesystem::path> objects;

public:
    explicit Compiler(build::BuildFile bfile);

    virtual void compile_to_objects() = 0;
    virtual void link_to_executable() = 0;
    virtual ~Compiler() = default;
};

class CompilerGCCStyle : public Compiler{
public:
    explicit CompilerGCCStyle(build::BuildFile bfile);

    void compile_to_objects() override;
    void link_to_executable() override;
};

class CompilerMSVCStyle : public Compiler{
public:
    explicit CompilerMSVCStyle(build::BuildFile bfile);

    void compile_to_objects() override;
    void link_to_executable() override;
};

class CompilerClangModules : public Compiler{
private:
    std::vector<module::ModuleInfo> modules;

public:
    explicit CompilerClangModules(build::BuildFile bfile);

    void compile_to_objects() override;
    void link_to_executable() override;
};

}
