#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <sstream>

enum class HttpVersion : int
{
    INVALIED = -1,
    HTTP1,
    HTTP1D0T1,
    HTTP2
};
static const char *method_str[] = {
    "INVALID",
    "GET",
    "POST"};
static const char *http_version_str[] = {
    "HTTP/1",
    "HTTP/1.1",
    "HTTP/2"
};
const char *http_verenum2str(HttpVersion &version)
{
    return http_version_str[static_cast<int>(version)];
}

HttpVersion http_str2verenum(const std::string &str)
{
    for (int i = 0; i < sizeof(http_version_str) / sizeof(char *); i++)
    {
        if (str == http_version_str[i])
        {
            return static_cast<HttpVersion>(i);
        }
    }
    return HttpVersion ::INVALIED;
}

static const char *http_code2status(short status_code)
{
    switch (status_code)
    {
    case 200:
        return " OK ";
    case 404:
        return " NOT FOUND ";
    case 304:
        return " REDIRECT ";
    default: {
        return " SERVER ERROR ";
    }
    }
    return "";
}

struct HttpReqProto {
    enum class Method : int {
        INVALIED =-1,
        GET,
        POST 
    };

    Method method;
    HttpVersion proto_ver;
    std::string route;
    std::unordered_map <std::string,std::string> context;
    std::string body;
    std::vector<std::string> route_params;
    
    static Method method2enum( std :: string & method ){
        for (std::size_t i = 0;i < sizeof(method_str) / sizeof(char*); i ++){
            if (strcmp(method_str[i],method.c_str()) == 0){
                return static_cast<Method>(i);
            }
        }
        return static_cast<Method>(-1);;
    }

    static const char *enum2method( Method & method ){
        return method_str[static_cast<int>(method)];
    }

    static HttpReqProto parse (const std::string& data) {
        HttpReqProto proto;
        // parse header 
        auto pos = data.find_first_of('\r');
        if(pos != std::string::npos && pos < data.size () -1 && data [ pos +1] =='\n') {
            std::string header{data.begin(),data.begin() + pos};
            std::stringstream ss{header};
            std::string method{};
            std::string proto_ver {};
            std::getline(ss,method,' ');
            std::getline(ss,proto.route,' ');
            std::getline(ss,proto_ver,' ');
            proto.method = HttpReqProto ::method2enum(method);
            proto.proto_ver = http_str2verenum(proto_ver);
            // parse context 
            auto context_start = pos + 2;
            auto context_end = data.find("\r\n\r\n");
            if(context_start < data.size () && context_end != std::string::npos) {
                std::string context_data(data.begin () + context_start,data.begin() + context_end);
                std::stringstream ss{context_data};
                std::string line{};
                while (std::getline(ss,line)){
                    auto colon_pos = line.find_first_of(':');
                    if (colon_pos == std::string::npos){
                        return {};
                    }
                    proto.context.insert({
                        {line.begin(),line.begin() + colon_pos},
                        {line.begin() + colon_pos + 1,line.end()}
                    });
                }
            }
            // parse body 
            auto body_start = context_end + 4;
            if (body_start < data.size()){
                proto.body = std::string{data.begin() + body_start,data.end()}; 
                return proto ;
            }
        }
    }

    std::string to_string() {
        std::stringstream ss{};
        ss << enum2method(method) << " " << route << " " << http_verenum2str(proto_ver)
            <<"\r\n";
        for (auto& p : context){
            ss << p.first << ":" << p.second << "\r\n";
            ss << "\r\n";
            ss << body ;
            return ss . str ();
        }
        return "";
    }
    void insert_context(const std::string& key,const std::string& val) {
        context.insert({key,val});
    }
};

struct HttpResProto {
    enum class Method : int {
        IVALIED = -1,
        GET = 0,
        POST 
    };

    HttpResProto() = default;
    HttpResProto(short status_code) 
        : status_code_ {status_code} {
    
    }
    
    HttpResProto(std::string body)
        :body_ {body} {
        content_.insert({"Content-Type","text/html;charset=UTF-8"});
    }

    void set_status_code(short code) {
        status_code_ = code;
    }

    HttpVersion proto_ver() {
        return proto_ver_;
    }
 
    std::unordered_map<std::string,std::string>& content(){
        return content_;
    }

    void set_body(std::string body) {
        body_ = std::move(body);
    }

    const std::string& body(){
        return body_;
    }

    std::string to_string() {
        std::stringstream ss{};
        ss << http_verenum2str(proto_ver_) << " " << status_code_ << " " 
            << http_code2status(status_code_)
            << "\r\n";
        if(body_.size() > 0 && content_.count("Content-Length") <= 0) {
            content_.insert({"Content-Length",std::to_string(body_.size())});
        }
        for (auto& p :content_) {
            ss << p.first << ":" << p.second << "\r\n";
        }
        ss << "\r\n";
        ss << body_;
        return ss.str();
    }
    void insert_content(const std::string& key,const std::string& val) {
        content_.insert({key,val});
    }
 private :
    HttpVersion proto_ver_ = HttpVersion::HTTP1D0T1;
    std::unordered_map<std::string,std::string> content_;
    std::string body_;
    short status_code_ = 200;
};
