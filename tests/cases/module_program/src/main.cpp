import funcs;
import math;

#include <iostream>

using math::constants::PI;

constexpr int val = math::add(2, 2);
constexpr double pwr = math::power(PI, 2);

int main(){
    funcs::hello();
    std::cout << val << '\n';
    std::cout << PI << '\n';
    std::cout << pwr << '\n';
}