#pragma once

#include <filesystem>
#include <vector>

#include <command.hpp>
#include <buildfile.hpp>
#include <modules.hpp>

namespace compiler::cmd::gcc {

command::Command compile_object(const build::BuildFile&, const std::filesystem::path&, const std::filesystem::path&);
command::Command link_executable(const build::BuildFile&, const std::vector<std::filesystem::path>&);

namespace mod {

command::Command compile_object(const build::BuildFile&, const module::ModuleInfo&, const std::vector<module::ModuleInfo>&);
command::Command link_executable(const build::BuildFile&, const std::vector<module::ModuleInfo>&);

}

}

namespace compiler::cmd::msvc {

command::Command compile_object(const build::BuildFile&, const std::filesystem::path&, const std::filesystem::path&);
command::Command link_executable(const build::BuildFile&, const std::vector<std::filesystem::path>&);

}
