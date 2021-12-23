#include "family_financial_manager.h"
#include <algorithm>
#include <iterator>
namespace ffms {

static int EAR_MAX_NO = 0;
static int SAVING_MAX_NO = 0;

inline int get_next_ear_no() {
    return ++ EAR_MAX_NO;
}

inline int get_next_saving_no() {
    return ++ SAVING_MAX_NO;
}

// in range
inline bool in_range(long x,long lower,long higher) {
    return x >= lower && x <= higher; 
}

// number valid
inline bool check_number(std::string &s) {
    for(auto &c : s) {
        if(!in_range(c,'0','9')) return false; 
    }
    return true;
}

#define CHECK_NUMBER(n) if(!check_number(n)) {std::cout << "invalid number\n";std::cin.sync();return;}

// load ear.csv data
static std::vector<expenses_and_receipts> load_expenses_and_receipts() {
    std::ifstream in{};
    in.open("./ears.csv");
    if(!in.is_open()) throw std::runtime_error("not found file ./ear.csvs");

    std::vector<expenses_and_receipts> ears{};
    std::string line {};
    while(std::getline(in,line)) {
        if(line.empty()) continue;
        std::stringstream ss{line};
        std::string no;
        std::string name;
        std::string amount;
        std::string comment;
        std::string date;
        std::string type;
        std::getline(ss,no,',');
        std::getline(ss,name,',');
        std::getline(ss,amount,',');
        std::getline(ss,type,',');
        std::getline(ss,comment,',');
        std::getline(ss,date,',');
        int ino = atoi(no.c_str());
        int itype = atoi(type.c_str());
        double damount = strtod(amount.c_str(),nullptr);
        ears.emplace_back(
            ino,
            name,
            damount,
            static_cast<expenses_and_receipts::Type>(itype),
            comment,date);
        if(ino > EAR_MAX_NO) EAR_MAX_NO = ino;
    }

    return ears;
}

// load savings data
static std::vector<saving> load_savings() {
    std::ifstream in{};
    in.open("./savings.csv");
    if(!in.is_open()) throw std::runtime_error("file savings.csv not found");
    std::vector<saving> savings{};
    std::string line{};
    while(std::getline(in,line)) {
        if(line.empty()) continue;
        std::stringstream ss{line};
        std::string no;
        std::string name;
        std::string amount;
        std::string saving_period;
        std::string interest_rate;
        std::string date;

        std::getline(ss,no,',');
        std::getline(ss,name,',');
        std::getline(ss,amount,',');
        std::getline(ss,saving_period,',');
        std::getline(ss,interest_rate,',');
        std::getline(ss,date,',');

        int ino = atoi(no.c_str());
        double damount = std::strtod(amount.c_str(),nullptr);
        int isaving_period = atoi(saving_period.c_str());
        double dinterest_rate = std::strtod(interest_rate.c_str(),nullptr);

        savings.emplace_back(ino,name,damount,isaving_period,dinterest_rate,date);
        if(ino > SAVING_MAX_NO) SAVING_MAX_NO = ino;
    }
    return savings;
}
// menu
static void menu() {
    std::cout << "*****************************************************\n"
              << "* Welcome to the family financial management system *\n"
              << "*****************************************************\n"
              << "* 1.show expenses and receipts                      *\n"
              << "* 2.show savings                                    *\n"
              << "* q.exit                                            *\n"
              << "*****************************************************\n";
}

// ctor. initialize expenses_and_receipts data and savings data.
family_financial_manager::family_financial_manager()
    :ears_{load_expenses_and_receipts()},
    savings_{load_savings()} {
    
    for(auto &ear : ears_) {
        if(accounts_.count(ear.name()) > 0) {
            accounts_[ear.name()] += ear.amount();
            continue;
        }
        accounts_.insert({ear.name(),ear.amount()});
    }
}

// main loop
void family_financial_manager::loop() {
    menu();
    char ch;
    std::cout << ">";
    while((ch = std::cin.get()) && ch != 'q') {
        std::cin.sync();
        switch(ch) {
            case '1':expenses_and_receipts_list();break;
            case '2':saving_list();break;
            default:break;
        }
        menu();
        std::cout << ">";
    }
    std::cout << "bye\n";
}

void family_financial_manager::insert_ear() {
    std::string name;
    std::string comment;
    std::string samount;
    double amount;

    std::cout << "please input name:";
    std::getline(std::cin,name);
    std::cout << "please input amount:";
    std::cin >> samount;
    if(!check_number(samount)) {
        std::cout << "invalid number\n";
        std::cin.sync();
        return;
    }
    std::cin.sync();
    std::cout << "please input comment:";
    std::getline(std::cin,comment);
    amount = strtod(samount.c_str(),nullptr);
    insert_ear(name.substr(0,10),amount,comment.substr(0,10));
    std::cin.sync();
}

void family_financial_manager::insert_ear(const std::string &name,double amount,const std::string &comment) {
    const time_t t = time(nullptr);
    tm *local = localtime(&t);
    char buf[64] = {0};
    strftime(buf,sizeof buf,"%Y-%m-%d %H:%M:%S",local);
    ears_.emplace_back(get_next_ear_no(),name,amount,static_cast<expenses_and_receipts::Type>((amount >= 0 ? 1 : 0)),comment,buf);
}

void family_financial_manager::insert_saving() {
    std::string name;
    std::string samount;
    std::string ssaving_preiod;
    double interest_rate;
    std::cout << "please input name:";
    std::cin.sync();
    std::getline(std::cin,name);
    std::cout << "please input amount:";
    std::cin.sync();
    std::cin >> samount;
    CHECK_NUMBER(samount);
    std::cout << "please input saving preiod(day):";
    std::cin >> ssaving_preiod;
    CHECK_NUMBER(ssaving_preiod);
    std::cout << "please input saving interest rete:";
    std::cin >> interest_rate;

    int saving_preiod = strtod(ssaving_preiod.c_str(),nullptr);
    double amount = strtod(samount.c_str(),nullptr); 
    if(accounts_.count(name) > 0 && amount <= accounts_[name]) {
        insert_saving(name,amount,saving_preiod,interest_rate / 100);
        accounts_[name] -= amount;
        insert_ear(name,-amount,"saving");
    }
    else {
        std::cout << "Insufficient Balance\n";
    }
    std::cin.sync();
}

void family_financial_manager::insert_saving(const std::string &name,double amount,int saving_preiod,double interest_rate) {
    const time_t t = time(nullptr);
    tm *local = localtime(&t);
    char buf[64] = {0};
    strftime(buf,sizeof buf,"%Y-%m-%d %H:%M:%S",local);
    savings_.emplace_back(get_next_saving_no(),name,amount,saving_preiod,interest_rate,std::string(buf));
}

void family_financial_manager::expenses_and_receipts_list() {
    std::cout << "input `n` jump to next page. input `p` jump to pre page. input `i` to insert one record.\n";
    std::cout << "input `e` to query expense records. input `r` to query receipt record. input `q` return.\n";
    expenses_and_receipts_one_page();
    char ch;
    // todo 
    // search record by type
    // expenses 支出
    // receipts 收入
    std::cout << ">";
    while((ch = std::cin.get()) && ch != 'q') {
        std::cin.sync(); // skip '\n'
        switch(ch) {
            case 'n':
                ear_current_page_ = 
                    ear_current_page_ >= std::count_if(
                            ears_.begin(),
                            ears_.end(),
                            [this](expenses_and_receipts &ear) {
                                return ear.type() == this->ear_search_type() 
                                    || this->ear_search_type() == expenses_and_receipts::Type::all;}) / page_size_ 
                        ? ear_current_page_ : ear_current_page_ + 1;
                break;
            case 'p':
                ear_current_page_ = ear_current_page_ - 1 < 0 ? 0 : ear_current_page_ - 1;
                break;
            case 'i':
                insert_ear();
                break;
            case 'd':
                delete_one_record_by_no();
                break;
            case 'e':
                ear_sreach_type_ = expenses_and_receipts::Type::expenses;
                ear_current_page_ = 0;
                break;
            case 'r':
                ear_sreach_type_ = expenses_and_receipts::Type::receipts;
                ear_current_page_ = 0;
                break;
            case 'a':
                ear_sreach_type_ = expenses_and_receipts::Type::all;
                ear_current_page_ = 0;
                break;
            default:break;
        }
        expenses_and_receipts_one_page();
        std::cout << ">";
    }
    std::cin.sync();
}

void family_financial_manager::delete_one_record_by_no() {
    std::cout << "please input no of record to delete:";
    std::string sno;
    std::cin >> sno;
    CHECK_NUMBER(sno);
    
    int no = atoi(sno.c_str());
    auto iter = std::find_if(ears_.begin(),ears_.end(),[no](expenses_and_receipts &ear) {return ear.no() == no;});
    if(iter != ears_.end()) {
        std::cout << "are you sure to delete the record of no? [y/n]";
        char ch;
        std::cin >> ch;
        if(ch == 'y') ears_.erase(iter);
    }
    std::cin.sync();
} 

void family_financial_manager::saving_list() {
    std::cout << "input `n` jump to next page. input `p` jump to pre page. input `i` to insert one record.\n";
    saving_one_page();
    char ch{};
    std::cout << ">";
    while((ch = std::cin.get()) && ch != 'q') {
        std::cin.sync(); // skip '\n'
        switch(ch) {
            case 'n':
                saving_current_page_ = 
                        saving_current_page_ >= savings_.size() / page_size_ ? saving_current_page_ : saving_current_page_ + 1;
                break;
            case 'p':
                saving_current_page_ = saving_current_page_ - 1 < 0 ? 0 : saving_current_page_ - 1;
                break;
            case 'i':
                insert_saving();
                break;
            case 'w':
                withdraw_saving();
                break;
            default:break;
        }
        saving_one_page();
        std::cout << ">";
    }
    std::cin.sync();
    // save_savings();
    // save_expenses_and_receipts();
}

void family_financial_manager::withdraw_saving() {
    std::cout << "please input the no of saving to be withdrawn:";
    std::string sno{};
    std::cin >> sno;
    CHECK_NUMBER(sno);
    int no = atoi(sno.c_str());
    std::cout << "Are you sure you want to withdraw this savings?[y/n]:";
    char ch;
    std::cin >> ch;
    if(ch == 'y') {
        auto iter = std::find_if(savings_.begin(),savings_.end(),[no](saving &s) {return s.no() == no;});
        if(iter != savings_.end()) {
            // now date
            time_t now{};
            time(&now);

            int year,month,day,hour,minute,second;
            sscanf(iter->date().c_str(),"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
            tm saved_date = {
                .tm_sec = second,
                .tm_min = minute,
                .tm_hour = hour,
                .tm_mday = day,
                .tm_mon = month - 1,
                .tm_year = year - 1900
            };

            // Calculate time difference
            double diff_secs = difftime(now,mktime(&saved_date));
            int income_days = (diff_secs / 86400) > iter->saving_period() ? iter->saving_period() : (diff_secs / 86400); 
            
            std::string comment = "withdraw";
            double interest = (iter->amount() * iter->interest_rate() * income_days);
            if(interest > 0) comment  = comment + (" (" + std::to_string(interest) + ")");
            insert_ear(iter->name(),iter->amount() + interest,comment); // calculate interest . Maximum  saved preiod * amount * interest rate
            iter = savings_.erase(iter);
            accounts_[iter->name()] += iter->amount();
        }
    }
    std::cin.sync();
}

void family_financial_manager::save_expenses_and_receipts() {
    std::ofstream os{};
    os.open("./ears.csv");
    for(auto &ear : ears_) os << ear.dump();
}

void family_financial_manager::save_savings() {
    std::ofstream os{};
    os.open("./savings.csv");
    for(auto &saving : savings_) os << saving.dump();
}

}