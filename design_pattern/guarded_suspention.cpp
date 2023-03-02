#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <iostream>
#include <cassert>

namespace cc {

template <typename T> class queue
{
  public:
    queue(int size) : size_(size) {}

    bool put(T&& data) {
      if (size_ <= deque_.size()) return false;

      deque_.emplace_back(std::forward<T>(data));
      return true;
    }

    bool get(T& data) {
      if (deque_.empty()) return false;

      data = std::move(deque_.front());
      deque_.pop_front();
      return true;
    }

    bool empty() const { return deque_.empty(); }

  private:
    int size_;
    std::deque<T> deque_;
};

class printer
{
  public:
    printer(int queue_size)
      : is_termination_requested_(false)
      , queue_(queue_size)
      , thread_(main_)
    {}

    ~printer() {
      {
        std::unique_lock<std::mutex> ul(mutex_);
        is_termination_requested_ = true;
      }
      cv_.notify_one();
      thread_.join();
    }

    bool append(std::string&& str) {
      {
        std::unique_lock<std::mutex> ul(mutex_);
        if (!queue_.put(std::move(str))) { return false; }
      }
      cv_.notify_one();
      return true;
    }

  private:
    std::function<void()> main_ = [this]
    {
      while (1) {
        std::string str;
        {
          std::unique_lock<std::mutex> ul(mutex_);
          while (queue_.empty()) {
            if (is_termination_requested_) { return; }
            cv_.wait(ul);
          }
          const bool result = queue_.get(str);
          assert(result);
        }
        std::cout << str << std::endl;
      }
    };

    bool is_termination_requested_;
    cc::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread thread_;
};

}// namespace cc

#include <sstream>

int main() {
  cc::printer printer(10);
  for (int i = 0; i < 1000; i++) {
    std::stringstream ss;
    ss << "hello world" << i;
    while (!printer.append(ss.str())) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}