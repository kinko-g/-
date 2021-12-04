#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
using namespace std;
class Observable;
class Observer : public enable_shared_from_this<Observer> {
	Observable *subject;
public:
	Observer() = default;
	~Observer() {}
	void addto(Observable *subject_);
	void update();
};
class Observable {
	vector<weak_ptr<Observer>> obs;
public:
	Observable() = default;
	~Observable() {}
	void reg(weak_ptr<Observer> ob) {
		obs.push_back(ob);
	}
	void notify() {
		auto iter = obs.begin();
		while(iter != obs.end()) {
			shared_ptr<Observer> sptr = iter->lock();
			if(sptr) {
				sptr->update();
				iter ++;
			}
			else {
				iter = obs.erase(iter);
			}
		}
	}
};

void Observer::addto(Observable *subject_) {
	//shared_from_this();
	subject_->reg(shared_ptr<Observer>(shared_from_this()));
	subject = subject_;
}
void Observer::update() {
	cout << "update\n";
}

int main()
{
	
	auto o1 = make_shared<Observer>();
	auto o2 = make_shared<Observer>();
	Observable obvable{};
	o1->addto(&obvable);
	o2->addto(&obvable);
	obvable.notify();
	cout << "Hello World";
   	return 0;
}