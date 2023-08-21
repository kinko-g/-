#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "http.h"
#include "response.h"

using Request = HttpReqProto;
class MetaService {
public :
    using ServiceType = std::function <Response(Request &)>;
    bool contains(const std::string& route) {
        return service_mapper_.count(route) > 0 || alias2main_.count(route);
    }
public:
    template <class F>
    void add_service(const std::string& url ,F&& f,std::vector<const char*>&& alias) {
        for(auto& alia : alias) {
            alias2main_.insert({std::string(alia),url});
        }
        service_mapper_.insert({url,[f](Request& req) -> Response {
            return f(req);
        }});
        for(auto& p : service_mapper_) {
            std::cout << "add service : " << p.first << "\n";
        }
    }
    Response invoke(const std::string& route,Request& req) {
        if (service_mapper_.count(route) > 0) {
            return service_mapper_[route](req);
        }
        else if(alias2main_.count(route) > 0) {
            std::cout << " route :" << route <<" alia call \n ";
             return service_mapper_[alias2main_[route]](req);
        }
        return nullptr;
    }
private:
    std::unordered_map<std::string,ServiceType> service_mapper_;
    std::unordered_map<std::string,std::string> alias2main_;
};

struct __init {
    template <class T,class F,class... Args>
    __init(std::string val,T that,F && f,Args... alias) {
        std::cout << val << "\t";
        std::cout << " alias :" << sizeof...(alias)<< "\n";
        std::vector<const char *> alia_arr = { alias ...};
        // for(auto& alia : alia_arr) {
        //     std::cout << " alia :" << alia << "\n";
        // }
        that->add_service(val,f,std::move(alia_arr));
    }
};

#define ROUTER(fn,url,...) private : __init __##fn##init{url,this,[this](Request& req) -> Response \
    { return this -> fn (req);},__VA_ARGS__};

class AppService final : public MetaService {
public:
    ROUTER(index,"/index","/");
    Response index(Request& req) {
        return HtmlResponse ("index.html");
    }
    ROUTER(home,"/home","/");
    Response home(Request &reg){
        return HtmlResponse ("home.html");
    }
    ROUTER(hot,"/hot"); 
    Response hot(Request& req) {
        return "<h1> Hot List </>"; 
    }
    ROUTER(favicon,"/favicon.ico"); 
    Response favicon(Request& req) {
        std::cout << "favicon\n";
        return "<h1>hello</>";
    }
    friend class __init;
};

class ServiceContainer {
public:
    ServiceContainer () = default;
    template<class T,class... Args>
    auto add_service(Args ...args)
        -> decltype(std::enable_if_t<std::is_base_of<MetaService,T>::value,int>{},(*this)) {
        auto serv_sp = std::make_shared<T>(args...);
        services_.push_back(serv_sp);
        return *this;
    }
    Response dispatch(Request& req) {
        std::cout << " dispatch \n";
        for(auto& serv : services_) {
            if (serv->contains(req.route)) {
                return serv->invoke(req.route,req);
            }
            // throw error 404
        }
        std::cout << req.route << " not found\n";
        return nullptr;
    }
private:
    std::vector<std::shared_ptr<MetaService>>services_;
};
