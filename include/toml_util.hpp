#pragma once

#include <iostream>
#include <vector>

#include <toml++/toml.hpp>

namespace toml_util {

std::vector<std::string> toml_array_to_vec(const toml::node_view<const toml::node>& array);
toml::table copy_toml_table(const toml::table& tbl, const std::string& name);

}
