#include <toml_util.hpp>

//#define FMT_HEADER_ONLY
#include <fmt/core.h>

using std::vector;
using std::string;

using NodeView = toml::node_view<const toml::node>;

namespace toml_util {

vector<string> toml_array_to_vec(const NodeView& array){
    vector<string> temp;

    if(array.type() != toml::node_type::array)
        throw std::runtime_error("Couldnt parse option in build file: option is not array");

    const auto arr = array.as_array();
    if(!arr)
        throw std::runtime_error("Couldnt get option that should be array");

    for(const auto& elem : *arr){
        if(elem.type() != toml::node_type::string)
            throw std::runtime_error("Couldnt parse option in build file: option should have only strings");

        if(const auto res = elem.value<string>(); res.has_value())
            temp.push_back(res.value());
        else
            throw std::runtime_error("Couldnt get element in option");
    }

    return temp;
}

toml::table copy_toml_table(const toml::table& tbl, const string& name){
    if(const auto* table_copy = tbl[name].as_table()){
         return *table_copy;
    }
    throw std::runtime_error(fmt::format("Couldnt copy table \"{}\"", name));
}

}
