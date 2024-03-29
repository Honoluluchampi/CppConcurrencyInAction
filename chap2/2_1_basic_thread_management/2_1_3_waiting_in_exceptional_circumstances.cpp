#include <thread>
#include <iostream>

void do_something(const int& i){std::cout << i << std::endl;}

struct func
{
    const int& i;
    func(const int& i_):i(i_){}
    void operator()()
    {
        for(unsigned j=0; j<1000000; ++j){
            do_something(i);
        }
    }
};


// use try/catch block to ensure that a thread with accesss to local
// state is finished vefore th function exits, whether th function exits normally, or by an exception
// but it's a little verbose
void f()
{
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_func);
    try{
        do_something(1);
    }
    catch(...){
        t.join();
        throw;
    }
    t.join();
}

// creating class doing join() in destructor
class thread_guard
{
    std::thread& t;
public:
    explicit thread_guard(std::thread& t_) : t(t_){}
    ~thread_guard()
    {
        if(t.joinable()) t.join();
    }
    // copy constructor and copy assigment operatos are not automatically
    // generated by a compilier. copying such a thread is dangerous.
    thread_guard(thread_guard const&)=delete;
    thread_guard& operator=(thread_guard const&)=delete;
};

void f_with_thread_guard()
{
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard tg(t);
    for(unsigned j = 0; j<100; j++){
        do_something(j);
    }
}