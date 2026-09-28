module;
#include <iostream>
#include <type_traits>
export module math;

export namespace math::constants {
constexpr double PI = 3.14;
}

export namespace math {

template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template<Number T>
constexpr T add(const T& a, const T& b){
    return a + b;
}

template<Number T>
constexpr T power(const T& value, const int& pow){
    T result = 1;
    for(int i = 0; i < pow; i++)
        result = result * value;
    return result;
}

}