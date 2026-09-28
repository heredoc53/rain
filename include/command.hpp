#pragma once

#include <iostream>
#include <tuple>
#include <vector>

namespace command {

struct Command{
    std::string name;
    std::vector<std::string> args;
};

[[deprecated("Use execute_command(const Command&) instead")]]
std::tuple<std::string, std::string, int> execute_command(const std::string& com);
std::tuple<std::string, std::string, int> execute_command(const Command& com);

}
