#include <http.h>

HttpReqProto HttpReqProto::parse (const std::string& data) {
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

std::string HttpReqProto::to_string() {
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

std::string HttpResProto::to_string() {
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