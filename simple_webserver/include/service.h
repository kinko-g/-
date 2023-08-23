#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "http.h"
#include "response.h"

inline constexpr bool is_ambiguous_route(const char* route) {
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
    std::string contains_pattern(Request& req,std::vector<std::string>& route_split);
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
    }
    Response invoke(const std::string& route,Request& req) {
        if (service_mapper_.count(route) > 0) {
            return service_mapper_[route].cb(req);
        }
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
    Response dispatch(Request& req);
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
        std::string filename = "img.png";
        std::string type = "png";
        if(req.route_params.size() > 0) {
            filename = "img1.jpeg";
        }
        ImageResponse res(filename);
        res.set_type(type);
        return res;
    }
    friend class __init;
};

