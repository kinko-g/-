#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

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

inline const char *http_verenum2str(HttpVersion &version)
{
    return http_version_str[static_cast<int>(version)];
}

inline HttpVersion http_str2verenum(const std::string &str)
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

    static HttpReqProto parse (const std::string& data);
    std::string to_string();

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

    std::string to_string();
    
    void insert_content(const std::string& key,const std::string& val) {
        content_.insert({key,val});
    }
 private :
    HttpVersion proto_ver_ = HttpVersion::HTTP1D0T1;
    std::unordered_map<std::string,std::string> content_;
    std::string body_;
    short status_code_ = 200;
};
