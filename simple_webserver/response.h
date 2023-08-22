#pragma once
#include "http.h"
#include <memory>
#include <string>
#include <fstream>
#include <cstdlib>

enum ContentType
{
    HTML,
    JSON,
    IMAGE
};

class AbstractResponse
{
public:
    AbstractResponse() = default;
    virtual ~AbstractResponse() = default;

    AbstractResponse(const std::string &content) {
        proto_.insert_content("ContentType", "text/html");
        proto_.set_body(content);
    }

    virtual void setResContent(std::string &content) {
        proto_.set_body(content);
    }
    ContentType virtual content_type() {
        return ContentType::HTML;
    }
    HttpResProto &proto() {
        return proto_;
    }

protected:
    HttpResProto proto_;
};

template <ContentType type>
class AbstractResponseProxy : public AbstractResponse
{
public:
    virtual ContentType content_type() override
    {
        return type;
    }
};

class HtmlResponse final : public AbstractResponseProxy<HTML>
{
public:
    HtmlResponse()
    {
        proto_.insert_content("Content-Type","text/html");
    }

    HtmlResponse(const std ::string filename) : HtmlResponse() {
        std ::fstream fs{};
        std ::string root_dir = "./Www/";
        auto evn_root_dir = std::getenv("root_dir");
        if (evn_root_dir)
        {
            root_dir = evn_root_dir;
            std::cout << " root dir :" << (root_dir + filename) << "\n";
            fs.open(root_dir + filename);
            if (fs.is_open())
            {
                fs.seekg(0, fs.end);
                auto sz = fs.tellg();
                fs.seekg(0, fs.beg);
                std ::string buf(sz, 0);
                fs.read(&*buf.begin(), sz);
                setResContent(buf);
            }
            else
            {
                std ::cout << " open file fault \n";
                std ::string content = "<h1>500 Server Error </>";
                setResContent(content);
            }
        }
    }
    virtual void setResContent(std ::string &content) override
    {
        proto_.set_body(content);
    }
};

class TextResponse final : public AbstractResponseProxy<HTML>
{
public:
    TextResponse()
    {
        proto_.insert_content("Content-Type", "text/html");
    }
    virtual void setResContent(std ::string &content) override
    {
        proto_.set_body(content);
    }
};

class ImageResponse final : public AbstractResponseProxy<IMAGE>
{
public:
    explicit ImageResponse(std ::string &filename) {
        proto_.insert_content("Content-Type", "image/png");
        std::fstream fs{};
        std::string root_dir = "./www/src/";
        auto evn_root_dir = std::getenv("source_dir");
        if (evn_root_dir) {
            root_dir = evn_root_dir;
            fs.open(root_dir + filename, std::ios::binary | std::ios::in);
            if (fs.is_open()) {
                fs.seekg(0, fs.end);
                auto sz = fs.tellg();
                fs.seekg(0, fs.beg);
                std ::string buf(sz, 0);
                fs.read(&*buf.begin(), sz);
                setResContent(buf);
            }
            else {
                std ::cout << " open file fault \n";
                std ::string content = "";
                setResContent(content);
            }
        }
    }
};

class JsonResponse final : public AbstractResponseProxy<JSON>{
public:
    JsonResponse(std::unordered_map<std::string,std::string> json) {

    }
};

class Response {
public:
    Response(const char* html) {
        response_ = std::make_shared<HtmlResponse>();
        std::string content(html);
        static_cast<HtmlResponse*>(response_.get())->setResContent(content);
    }
       
    Response(std ::string html) {
        response_ = std::make_shared<HtmlResponse>(html);
    }

    Response(std::unordered_map<std::string,std::string> json) {
        response_ = std::make_shared<JsonResponse>(json);
    }
    Response(JsonResponse res) {
        response_ = std::make_shared<JsonResponse>(res);
    }
    Response(HtmlResponse res) {
        response_ = std::make_shared<HtmlResponse>(res);
    }
    Response(ImageResponse res) {
        response_ = std::make_shared<ImageResponse>(res);
    }
    Response(std::nullptr_t) {
        response_ = std::make_shared<AbstractResponse>();
    }

    ContentType content_type() {
        return response_->content_type();
    }

    bool is_null() {
        return !response_;
    }

    HttpResProto &proto(){
        return response_->proto();
    }
private:
    std::shared_ptr<AbstractResponse> response_;
};