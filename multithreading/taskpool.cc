#include <iostream>
#include <functional>
#include <atomic>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
class TaskPool {
    using Task = std::function<void()>;
    std::atomic<bool> stoped;
    std::vector<std::thread> pool;
    std::queue<Task> tasks;
    std::mutex m_lock;
    std::condition_variable cv_task;
    std::atomic<int> idl_thread_num;
public:
    TaskPool(unsigned short size = 4):stoped{false} {
        idl_thread_num = size < 1 ? 1 : size;
        for(int i = 0;i < idl_thread_num;i++) {
            pool.emplace_back(
                [this]{
                    while(!this->stoped)  {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock{this->m_lock};
                            this->cv_task.wait(lock,[this]{return this->stoped.load() || !this->tasks.empty();});
                            if(this->stoped && this->tasks.empty()) {
                                return ;
                            }
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                        this->idl_thread_num--;
                        task();
                        this->idl_thread_num++;
                    }
                }
            );
        } 
    }

    template<typename F,typename... Args>
    auto commit(F&&f,Args...args) -> std::future<decltype(f(args...))> {
        if(this->stoped.load())
            throw std::runtime_error("commit on threadpool is stoped!");
        using result_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<result_type ()> >( //太鸡巴有迷惑性了 这个 result_type () *取返回类型 + () 成为指针 指向 bind 后的函数
            std::bind(std::forward<F>(f),std::forward<Args>(args)...)
        );
        std::future<result_type> ans = task->get_future();
        {
            std::lock_guard<std::mutex> lock(this->m_lock);
            tasks.emplace(
                [task]{
                    (*task)();
                }
            );
        }
        cv_task.notify_one();
        return ans;
    }

    int get_idl_thread_num() {
        return this->idl_thread_num;
    }

    ~TaskPool() {
        stoped.store(true);
        cv_task.notify_all();
        for(auto& thread:pool) {
            if(thread.joinable())thread.detach();
        }
    }
};

int func(int a,int b) {
    return a + b;
}

int main() {
    TaskPool t(3);
    auto result1 = t.commit(&func,1,2);
    auto result2 = t.commit(&func,2,2);
    auto result3 = t.commit(&func,3,2);
    std::cout << result1.get() <<std::endl;
    std::cout << result2.get() <<std::endl;
    std::cout << result3.get() <<std::endl;
    return 0;
}