#include <cstdio>
#include <cstdlib>

#include <platform.hpp>

using std::string;
using std::optional;

namespace platform {

optional<string> get_environment_variable(const string& var){
    const char* res = std::getenv(var.c_str());

    if(res == nullptr){
        return std::nullopt;
    }else{
        return string(res);
    }
}

}
