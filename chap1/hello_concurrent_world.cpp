#include <iostream>
#include <thread>

void hello()
{
    std::cout << "Hello Concurrent World!" << std::endl;
}

int main()
{
    // every thread has to hace an initial function
    // it should be specified in the constructor of a std::thread object
    // (initial function for initial thread is main())
    std::thread t(hello); // new thread starts at hello()(init func)
    // join() is here to make the calling thread (main()) to 
    // wait for the thred associated with the std::thread object (t)
    t.join();
}