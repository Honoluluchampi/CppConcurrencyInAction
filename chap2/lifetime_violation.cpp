#include <thread>
#include <iostream>

void do_something(int i)
{
    std::cout << i << std::endl;
}

struct func 
{
    int &i;
    func(int &i_) : i(i_){}
    void operator()()
    {
        for(unsigned j = 0; j<100;++j)
        {
            do_something(i);
        }
    }
};

void oops()
{
    int some_local_state = 0;
    func my_func(some_local_state); // func::i is a ref to local-valiable
    std::thread my_thread(my_func);
    my_thread.detach(); // decide to detach the thread
    // when oops finish some_local_state is deleted, then func::i becomes undefined.
}

int main()
{
    oops();
    for(unsigned j = 0; j < 1000; ++j)
    {
        do_something(1);
    }
}