#include <thread>
#include <vector>
#include <iostream>
#include <queue>
#include <functional>
#include <future>

using task_t = std::function<void()>;

class thread_pool {
  public:
    thread_pool(int pool_size = -1) {
      // define pool sizé
      if (pool_size <= 0)
        pool_size_ = std::thread::hardware_concurrency();
      // setup worker threads
      threads_.resize(pool_size_);
      for (int i = 0; i < pool_size_; i++) {
        threads_[i] = std::thread(worker);
      }
    }

    ~thread_pool() {
      {
        std::lock_guard<std::mutex> lock(task_mutex_);
        running_ = false;
      }
      // wake up all threads
      cv_.notify_all();
      for (auto& thread : threads_) {
        thread.join();
      }
    }

    template <typename F>
    void push_task(const F& task) {
      {
        const std::lock_guard<std::mutex> lock(task_mutex_);

        if (!running_) {
          throw std::runtime_error("cannot push new task after shutdown.");
        }

        task_queue_.push(task_t(task));
      }

      cv_.notify_one();
    }

    template <typename F, typename... Args, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    std::future<R> submit(F& func, Args&&... args) {
      
      auto task = std::make_shared<std::packaged_task<R()>>(
        [&func, args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          return std::apply([&func](auto&& ...args) {
            return func(std::forward<Args>(args)...);
          }, std::move(args));
        }
      );

      auto future = task->get_future();

      push_task([task = std::move(task)]() { (*task)(); });
      return future;
    }

    int get_pool_size() const { return pool_size_; }
  
  private:
    task_t worker = [this] {
      while(true) {
        task_t task;
        {
          // wait until the task_queue is pushed some tasks
          std::unique_lock<std::mutex> lock(task_mutex_);
          cv_.wait(lock, [&] { return !task_queue_.empty() || !running_; });
        
          // finish if the pool is not running and if task queue has no tasks
          if (!running_ && task_queue_.empty()) return;

          // get a task from task queue
          task = std::move(task_queue_.front());
          task_queue_.pop();
        } // release mutex

        task();
      }
    };

    int pool_size_;
    std::vector<std::thread> threads_;
    std::queue<task_t> task_queue_;
    std::mutex task_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_ = true;
};

int some_process(int number) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return number;
}

int main() {
  thread_pool t_pool;
  std::cout << t_pool.get_pool_size() << std::endl;

  int task_count = 100;
  std::vector<std::future<int>> results(task_count);

  for (int i = 0; i < task_count; i++) {
    results[i] = t_pool.submit(some_process, i);
  }

  for (auto& result : results) {
    std::cout << result.get() << std::endl;
  }
}