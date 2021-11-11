#include <iostream>
#include <thread>

void do_something(){std::cout << "do_something" << std::endl;}
void do_something_else(){std::cout << "do_something_else" << std::endl;}

class background_task
{
public:
    void operator() () const
    {
        do_something();
        do_something_else();
    }
};

int main()
{
    // std::thread can be passed with any callable type
    // so we can pass an instance of a class with a function call operator
    background_task bt;
    // here, supplied function object is copied into the new thread's storage
    // be careful the copy behaves equivalently to the original or not
    std::thread my_thread(bt);
    my_thread.join();
    
    // C++'s most vexing parse
    //std::thread my_thread2(background_task()); // interpreted as function definition
    // alternatives
    std::thread my_thread2((background_task())); // one more parenthesis
    std::thread my_thread3{background_task()}; // uniform initialization syntax
    std::thread my_thread4([=]{do_something(); do_something_else();}); // lambda expression
    my_thread2.join();
    my_thread3.join();
    my_thread4.join();
}