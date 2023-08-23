#include <service.h>
std::string MetaService::contains_pattern(Request& req,std::vector<std::string>& route_split) {
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


Response ServiceContainer::dispatch(Request& req) {
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