#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include "expenses_and_receipts.h"
#include "saving.h"

namespace ffms {

class  family_financial_manager {
public:
    family_financial_manager();
    ~family_financial_manager() {
        save_savings();
        save_expenses_and_receipts();
    }
    void loop();
    void insert_ear();
    void save_savings();
    void save_expenses_and_receipts();
    void insert_ear(const std::string &name,double amount,const std::string &comment);
    void insert_saving();
    void insert_saving(const std::string &name,double amount,int saving_preiod,double interest_rate);
    void withdraw_saving();
    const std::vector<expenses_and_receipts>& get_expenses_and_receipts() const {return ears_;}
    const std::vector<saving>& get_savings() const {return savings_;}
    expenses_and_receipts::Type ear_search_type() {return ear_sreach_type_;}
    const std::map<std::string,double> accounts() const {return accounts_;}
    void expenses_and_receipts_list();
    void saving_list();
private:
    void delete_one_record_by_no();
    void expenses_and_receipts_one_page() {
        std::cout << "No." << "\t\t" << "NAME" << "\t\t" 
            << "AMOUNT" << "\t\t" << "Type" << "\t\t\t" << "DATE" << "\t\t\t\t\t" << "COMMENT\n";
        for(size_t i = ear_current_page_ * page_size_; i < ears_.size() && i < (ear_current_page_ + 1) * page_size_;i ++) {
            if(ear_sreach_type_ == expenses_and_receipts::Type::all || ears_[i].type() == ear_sreach_type_) std::cout << ears_[i];
        }
    }
    void saving_one_page() {
        std::cout << "No." << "\t\t" << "NAME" << "\t\t" 
            << "AMOUNT" << "\t\t" << "SAVING PREIOD" << "\t\t" << "INTEREST RATE" << "\t\t" << "DATE\n";
        for(size_t i = saving_current_page_ * page_size_; i < savings_.size() && i < (saving_current_page_ + 1) * page_size_;i ++) {
            std::cout << savings_[i];
        }
    }
private:
    std::map<std::string,double> accounts_;
    std::vector<expenses_and_receipts> ears_;
    std::vector<saving> savings_;
    expenses_and_receipts::Type ear_sreach_type_ = expenses_and_receipts::Type::all;
    int page_size_ = 10;
    int ear_current_page_ = 0;
    int saving_current_page_ = 0;
};
}