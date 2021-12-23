#include <iostream>
#include <cstring>
using namespace std;

class cow {
	struct cow_val {
		int cnt;
		char *val;
		cow_val(const char *s):cnt{1} {
			val = new char[strlen(s) + 1];
			strcpy(val,s);
		}
		~cow_val() {
			delete []val;
		}
	};
public:
	cow(const char *s = ""):value{new cow_val{s}} {}
	cow(const cow &c):value{c.value} {
		printf("%s\n",value->val);
		value->cnt ++;
	}
	cow& operator=(const cow &c) {
		if(&c == this) return *this;
		if(--value->cnt == 0) delete value;
		std::cout << "operator= \n";
		value = c.value;
		value->cnt ++;
		return *this;
	}
	const int use_count() const {return value->cnt;}
	const char* c_str() const {
		return value->val;
	}
	const char& operator[](int index) const {
		return value->val[index];
	}
	char& operator[](int index) {
		if(value->cnt > 1) {
			value->cnt --;
			value = new cow_val{value->val};
		}
		return value->val[index];
	}
	~cow() {
		if(--value->cnt == 0) delete value;
	}
private:
	cow_val *value;
};

int main()
{
   	//cout << "Hello World";
   	cow c1{"hello world"};
	cow c2 = c1;
	printf("%x\n",c1.c_str());
	printf("%x\n",c2.c_str());
	const char &tmp = c1[0];
	std::cout << tmp << "\n";
	printf("%x\n",c1.c_str());
	printf("%x\n",c2.c_str());
	return 0;
}