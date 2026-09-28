#include <funcs.hpp>
#include <foo.hpp>
#include <iostream>

struct test{
    int a;
    explicit test(int v) : a{v} {}
};

struct test2{
    test t;
};

void testing(){
    int a = 1;
    test t{a}; //works, gcc is version 12
    //test2 t2{ .t{a} }; // produce error on gcc if constructor is explicit, for clang it works
    //test2 t2{ .t = test{a} }; // works on both
}

void goodbye(){
    std::cout << "Goodbye\n";
    hello();
    farewell("foo");
}

