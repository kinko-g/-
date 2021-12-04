#include <iostream>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <assert.h>
class thread {
public:
    using function_type = std::function<void()>;
    template<class F,class ...Args>
    thread(F &&func,const std::string &name = "",Args ...args)
        :func_(std::bind(std::forward<F>(func),std::forward<Args>(args)...)),
        name_{name},
        started_{false},
        joined_{false},
        tid_{new pid_t{0}},
        pthread_id_{0} {
            created_num_ ++;
        }

    ~thread();
    void start();
    void join();
private:
    bool started_;
    bool joined_;
    std::string name_;
    function_type func_;
    std::shared_ptr<pid_t> tid_;
    pthread_t pthread_id_;
    static std::atomic<int> created_num_;
};

namespace current_thread {
    pid_t tid();
    const char* get_thread_name();
    thread_local const char *thread_name = "unkown";
}
namespace {
    thread_local pid_t cache_tid = 0;
    pid_t __gettid() {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }
    void after_fork() {
        cache_tid = __gettid();
        current_thread::thread_name = "main";
    }
    class register_after_fork {
    public:
        register_after_fork() {
            current_thread::thread_name = "main";
            pthread_atfork(nullptr,nullptr,&after_fork);
        }
    };
    struct thread_data {
        std::string name_;
        thread::function_type func_;
        std::weak_ptr<pid_t> wk_ptr_;

        thread_data(const thread::function_type &func,const std::string name,std::shared_ptr<pid_t> &sp_pid)
            :func_{func},
            name_{name},
            wk_ptr_{sp_pid} {

            }
        void run_in_thread() {
            auto sp_pid = wk_ptr_.lock();
            if(sp_pid) {
                *sp_pid = current_thread::tid();
                sp_pid.reset();
            }
            if(!name_.empty()) {
                current_thread::thread_name = name_.c_str();
            }
            ::prctl(PR_SET_NAME,current_thread::thread_name);
            func_();
            current_thread::thread_name = "finished";
        }
    };
}
register_after_fork raf{};

void* start_thread(void *obj) {
    auto td = static_cast<thread_data*>(obj);
    td->run_in_thread();
    delete td;
    return nullptr;
}
pid_t current_thread::tid() {
    if(cache_tid <= 0) cache_tid = __gettid();
    return cache_tid;
}
std::atomic<int> thread::created_num_ = 0;

void thread::start() {
    assert(!started_);
    started_ = true;
    auto data = new thread_data{func_,name_,tid_};
    if(::pthread_create(&pthread_id_,nullptr,&start_thread,data)) {
        started_ = false;
        delete data;
        abort();
    }
}
void thread::join() {
    assert(started_);
    assert(!joined_);
    pthread_join(pthread_id_,nullptr);
    joined_ = true;
}
thread::~thread() {
    if(started_ && !joined_) {
        pthread_detach(pthread_id_);
    }
}
int main() {
    thread t{[] {std::cout << "name" << "\n";}};
    t.start();
    t.join();
    return 0;
}

