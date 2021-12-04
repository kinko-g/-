#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>
#include <deque>
#include <memory>
#include <random>

template<class T,class Container = std::deque<T>>
class thread_safe_queue {
    Container queue;
    std::mutex m;
    std::condition_variable cv;
public:
    thread_safe_queue() = default;
    void push(T data) {
        std::lock_guard<std::mutex> lock{m};
        queue.push_back(data);
        cv.notify_one();
    }

    std::shared_ptr<T> wait_pop() {
        std::unique_lock<std::mutex> lock{m};
        cv.wait(lock,[this](){
            return !this->queue.empty();
        });
        auto data = queue.front();
        queue.pop_front();
        return std::shared_ptr<T>(new T(data));
    }
};

thread_safe_queue<int> q{};
std::default_random_engine e;
std::uniform_int_distribution<int> u(0,10);
using namespace std::literals::chrono_literals;
void comsumer() {
    while(1) {
        std::this_thread::sleep_for(1s);
        auto p = q.wait_pop();
        std::cout << "comsumer : " <<  *p << "\n";
    }
}

void producer() {
    while(1) {
        std::this_thread::sleep_for(1s);
        auto data = u(e);
        std::cout << "porducer : " << data << "\n";
        q.push(data);
    }
}

int main() {
    std::thread t1 {producer};
    comsumer();
    t1.join();
    return 0;
}

