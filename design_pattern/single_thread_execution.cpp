#include <thread>
#include <mutex>
#include <iostream>

int main () {
  
  auto f = [](int& count, std::mutex& mutex) {
    for (int n = 0; n < 1000000; n++) {
      std::lock_guard<std::mutex> lg(mutex);
      count++;
    }
  };

  int count = 0;
  std::mutex mutex;

  std::thread ta(f, std::ref(count), std::ref(mutex));
  std::thread tb(f, std::ref(count), std::ref(mutex));

  ta.join();
  tb.join();
  std::cout << count << std::endl;
}