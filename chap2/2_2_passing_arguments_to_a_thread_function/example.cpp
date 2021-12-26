#include <thread>
#include <iostream>
#include <string>

void f(std::string const& str)
{
    std::cout << &str << std::endl;
}

int main()
{
    // 0x7ffeeaf83ab0
    std::string str = "king";
    std::cout << &str << std::endl;
    // its possible to pass thread-function's argument like follwing form,
    // but all arguments are copied into internal storage as rvalue
    // as if they are temporary.

    // and string literal is passed to the function as const char*, so 
    // its important to cast to string when you pass it to the constructor of std::thread
    std::thread thr(f, str);
    // std::thread thr(f, static_cast<std::string>("hello"));
    // 0x7f975a405878
    thr.join();
}