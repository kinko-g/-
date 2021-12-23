#pragma once
#include <iomanip>
namespace ffms {
// 收支
class expenses_and_receipts {
public:
    enum Type {
        expenses,
        receipts,
        all
    };
public:
    expenses_and_receipts(
        int no,
        const std::string &name,
        double amount,
        Type type,
        const std::string &comment,
        const std::string &date
        )
        : no_{no},
        name_{name},
        amount_{amount},
        type_{type},
        comment_{comment},
        date_{date} {

    }
    const int no() const {return no_;}
    const double amount() const {return amount_;}
    const std::string& name() const {return name_;}
    const std::string& date() const {return date_;}
    const std::string& comment() const {return comment_;}
    const int type() const {return type_;}
    const std::string dump() const {
        char buf[1024] = {0};
        snprintf(buf,sizeof buf,"%d,%s,%g,%d,%s,%s\n",no_,name_.c_str(),amount_,static_cast<int>(type_),comment_.c_str(),date_.c_str());
        // return std::to_string(no_) + "," 
        //     + name_ + "," + std::to_string(amount_) + "," 
        //     + std::to_string(type_) + comment_ + "," + date_ ;
        return {buf};
    }
private:
    int no_;
    std::string name_;
    double amount_;
    std::string comment_;
    std::string date_;
    int type_;
};

static std::ostream& operator<<(std::ostream& os,expenses_and_receipts &ear) {
    os << ear.no() << "\t\t" << ear.name() << "\t\t" 
        << ear.amount() << "\t\t" << (ear.type() == expenses_and_receipts::Type::expenses ? "expenses" : "receipts") << "\t\t"
        << ear.date()  << "\t\t\t" << ear.comment() << "\n";
    return os;
}

}