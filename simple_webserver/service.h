#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "http.h"
#include "response.h"

static constexpr bool is_ambiguous_route(const char* route) {
    const char* iter = route;
    while(*iter) {
        if(*iter == '$') return true;
        iter ++;
    }
    return false;
}

using Request = HttpReqProto;
class MetaService {
public :
    using ServiceType = std::function <Response(Request &)>;
    struct RouteInfo {
        bool is_ambiguous;
        ServiceType cb;
    };
    bool contains(const std::string& route) {
        return service_mapper_.count(route) > 0 || alias2main_.count(route);
    }

    std::string contains_pattern(Request& req,std::vector<std::string>& route_split) {
        auto& route = req.route;
        // std::cout << "req.route : " << req.route << "\t" << service_mapper_.count(route) << "\n";
        // std::cout << alias2main_.count(route) << "\n";
        if(service_mapper_.count(route) > 0 && !service_mapper_[route].is_ambiguous) {
            return route;
        }
        if(alias2main_.count(route) > 0 && !service_mapper_[alias2main_[route]].is_ambiguous) {
            return alias2main_[route];
        }
        int i{};
        std::string word{};
        for(auto& p : service_mapper_) {
            std::cout << p.first << "\t " << p.second.is_ambiguous << "\n";
            if(!p.second.is_ambiguous) {
                continue;
            }
            i = 0;
            std::vector<std::string> route_params{};
            std::stringstream ss{p.first};
            while(std::getline(ss,word,'/')) {
                if(i >= route_split.size() || (word != route_split[i] && word != "$")) {
                    break;
                }
                std::cout << route_split[i] << "\t" << word << "\t" << i << "\n";
                if(word == "$") {
                    route_params.push_back(route_split[i]);
                }
                i ++;
            }
            std::cout << "i : " << i << "\n";
            if(i == route_split.size()) {
                std::cout << "pattern matched\n";
                req.route_params = std::move(route_params);
                return p.first;
            }
        }
        return "";
    }
public:
    template <class F>
    void add_service(std::string& url ,F&& f,std::vector<const char*>&& alias) {
        std::cout << "add service : " << url << "\n";

        if(url.back() == '/') {
            url.pop_back();
        }
        for(auto& alia : alias) {
            auto alia_s = std::string(alia);
            if(alia_s.size() > 1 && alia_s.back() == '/') {
                alia_s.pop_back();
            }
            alias2main_.insert({alia_s,url});
        }

        RouteInfo route_info {
            is_ambiguous_route(url.c_str()),
            [f](Request& req) -> Response {
                return f(req);
            }
        };
        service_mapper_.insert({url,std::move(route_info)});
        // for(auto& p : service_mapper_) {
        //     std::cout << "add service : " << p.first << "\n";
        // }
    }
    Response invoke(const std::string& route,Request& req) {
        if (service_mapper_.count(route) > 0) {
            return service_mapper_[route].cb(req);
        }
        // else if(alias2main_.count(route) > 0) {
        //     // std::cout << " route :" << route << " alia call \n ";
        //     return service_mapper_[alias2main_[route]].cb(req);
        // }
        return nullptr;
    }
private:
    std::unordered_map<std::string,RouteInfo> service_mapper_;
    std::unordered_map<std::string,std::string> alias2main_;
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
        std::stringstream ss{req.route};
        std::vector<std::string> route_split{};
        std::string param{};
        while(std::getline(ss,param,'/')) {
            std::cout << "route split : " << param << "\n";
            route_split.push_back(param);
        }
        std::string route{};
        for(auto& serv : services_) {
            route = serv->contains_pattern(req,route_split);
            std::cout << "find parttern route : " << route << "\n";
            if (!route.empty()) {
                return serv->invoke(route,req);
            }
        }
        std::cout << req.route << " not found\n";
        // throw error 404
        return nullptr;
    }
private:
    std::vector<std::shared_ptr<MetaService>>services_;
};

struct __init {
    template <class T,class F,class... Args>
    constexpr __init(std::string val,T that,F && f,Args... alias) {
        // std::cout << val << "\t";
        // std::cout << " alias :" << sizeof...(alias) << "\n";
        std::vector<const char *> alia_arr = { alias ...};
        that->add_service(val,f,std::move(alia_arr));
    }
};

#define ROUTER(fn,url,...) private : __init __##fn##init{url,this,[this](Request& req) -> Response \
    { return this -> fn (req);},__VA_ARGS__};

class AppService final : public MetaService {
public:
    ROUTER(index,"/index","/");
    Response index(Request& req) {
        return HtmlResponse("index.html");
    }
    ROUTER(home,"/home");
    Response home(Request &reg){
        return HtmlResponse("home.html");
    }
    ROUTER(hot,"/hot"); 
    Response hot(Request& req) {
        return "<h1> Hot List </>"; 
    }
    ROUTER(favicon,"/favicon.ico"); 
    Response favicon(Request& req) {
        std::cout << "favicon\n";
        std::string filename = "logo_64.png";
        return ImageResponse(filename);
    }
    ROUTER(image,"/image/$"); 
    Response image(Request& req) {
        for(auto& param : req.route_params) {
            std::cout << "param : " << param << "\n"; 
        }
        std::string filename = "img.png";
        return ImageResponse(filename);
    }
    friend class __init;
};

