#include <iostream>

#include <funcs.hpp>
#include <math.hpp>

int main(){
    std::cout << "Test\n";
    goodbye();
    std::cout << ret_add(65, 2) << ' ' << ret_mul(25, 2) << '\n';
    return 0;
}