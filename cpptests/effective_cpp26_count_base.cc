#include <iostream>
#include <vector>
#include <memory>


template<class T>
class base_count {
public:
    static int count() {
        return created_count;
    }
protected:
    base_count();
    base_count(const base_count&);
    ~base_count() {created_count --;}
    static int created_count;
    static int create_max;
private:
    void init() {
        if(created_count >= create_max) throw std::runtime_error("more than create_max");
        created_count ++;
    }
    
};

template<class T>
int base_count<T>::created_count = 0;
template<class T>
int base_count<T>::create_max = 10;

template<class T>
base_count<T>::base_count() {
    init();
}
template<class T>
base_count<T>::base_count(const base_count<T>& rhs) {
    init();
}

class printer : private base_count<printer> {
public:
    static std::shared_ptr<printer> get_instance() {
        return std::shared_ptr<printer>(new printer{});
    }
    using base_count<printer>::created_count;
    using base_count<printer>::create_max;
private:
    printer() = default;
    printer(const printer&) = delete;
    
};

int main() {
    auto p = printer::get_instance();
    auto p1 = printer::get_instance();
    std::cout << base_count<printer>::count();
    return 0;
}

