#pragma once
#include <string>
namespace ffms {
// 储蓄
class saving {
public:
    saving(int no,
        const std::string &name,
        double amount,
        int saving_period,
        double interest_rate,
        const std::string &date)
        :no_{no},
        amount_{amount},
        name_{name},
        date_{date},
        saving_period_{saving_period},
        interest_rate_{interest_rate} {

    }
    saving() = default;
    const int no() const {return no_;}
    const std::string& name() const {return name_;}
    const std::string& date() const {return date_;}
    const double amount() const {return amount_;}
    const int saving_period() const {return saving_period_;}
    const double interest_rate() const {return interest_rate_;}
    
    std::string dump() {
        char buf[1024] = {0};
        snprintf(buf,sizeof buf,"%d,%s,%g,%d,%g,%s\n",no_,name_.c_str(),amount_,saving_period_,interest_rate_,date_.c_str());
        return {buf};
    }
private:
    int no_; // 序号
    double amount_; // 金额
    std::string name_; // 存储人
    std::string date_; // 日期
    int saving_period_; // 存期
    double interest_rate_; // 利率
};

static std::ostream& operator<<(std::ostream& os,saving &s) {
    os << s.no() << "\t\t" << s.name() << "\t\t" << s.amount() << "\t\t"
        << s.saving_period() << "\t\t\t" << s.interest_rate() << "\t\t\t" << s.date() << "\n";
    return os;
}
}