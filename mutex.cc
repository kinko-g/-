#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <iterator>
#include <condition_variable>
#include <deque>
#include <string>
#include <random>
std::mutex mtx;
std::condition_variable cond;
std::deque<int> q;
int count = 0;
void test() {
    for(int i = 0;i < 20; i++) {
        std::lock_guard<std::mutex> lc(mtx);
        std::cout << "tid : " << std::this_thread::get_id() 
            << "i : " << i << "\n";
    }
}
void insert(std::vector<int>& vec) {
    std::unique_lock<std::mutex> ul(mtx,std::defer_lock);
    ul.lock();
    ul.unlock();
    for(int i = 0; i < 100; i++) {
        ul.lock();
        vec.push_back(i);
    }
}// 自动解锁

void produce() {
    using namespace std::literals::chrono_literals;
    int data;
    std::default_random_engine e(10);
    std::uniform_int_distribution<unsigned> u(0,100);
    while(1) {
        if(count < 10) {
            data = u(e);
            std::unique_lock<std::mutex> ul(mtx);
            q.push_front(data);
            std::cout << "produce data: " << data << "\n";
            cond.notify_one();
            count++;
        }
        std::this_thread::sleep_for(1s);
    }
}

void consumer() {
    using namespace std::literals::chrono_literals;
    int data;
    while(1) {
        std::unique_lock<std::mutex> ul(mtx);
        while(q.empty()) {
            cond.wait(ul);
        }
        //cond.wait(ul,[](){return !q.empty();});
        data = q.back();
        q.pop_back();
        std::cout << "consumer data: " << data << "\n";
        count--;
        std::this_thread::sleep_for(2s);
    }
}

int main() {
    // std::vector<int> vec{};
    // std::thread t1(insert,std::ref(vec));
    // std::thread t2(insert,std::ref(vec));
    // t1.join();
    // t2.join();
    // std::copy(vec.begin(),vec.end(),std::ostream_iterator<int>(std::cout," "));
    // std::cout<<std::endl;
    std::thread t1(produce);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}