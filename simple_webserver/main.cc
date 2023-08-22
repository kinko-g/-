#include <cstdio>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <functional>
#include <set>
#include <queue>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <functional>
#include <fstream>
#include <type_traits>
#include "service.h" 
#include "http.h"

using namespace boost;

class NonCopyable {
public :
    NonCopyable ()= default ;
    NonCopyable ( const NonCopyable &) = delete;
    NonCopyable & operator =( const NonCopyable &) = delete;
    virtual ~ NonCopyable ()= default ;
};

struct ResProtoNode {
    size_t rw_sz = 0;
    std::string data;
};

class Session : public NonCopyable, public std::enable_shared_from_this<Session> {
public:
    using OnMessageCallbackType = std :: function <void(std::shared_ptr<Session>,HttpReqProto&)>;
    using OnCloseCallbackType = std :: function <void(std::shared_ptr<Session>)>;
    Session() = delete;

    Session(asio::io_context& ioc)
        : ioc_{ioc},
        sock_(ioc_),
        rd_buf_(1024,0) {

    }

    asio::ip::tcp::socket& socket() {
        return sock_;
    }

    void set_uuid (std::string uuid) {
        uuid_ = std::move(uuid);
    }

    const std::string& uuid() const { 
        return uuid_;
    }
    void start() {
        sock_.async_read_some(asio::buffer(rd_buf_,rd_buf_.size()),
            std::bind(&Session::do_read,this,std::placeholders::_1,std::placeholders::_2));
    }

    void do_read(const system::error_code& ec,size_t bytes_transed) {
        if(!ec) {
            auto data = std::string(rd_buf_.begin(),rd_buf_.end());
            auto proto = HttpReqProto::parse(data);
            std::cout << " bytes transed : "<< bytes_transed << "\n";
            std::cout << " recved proto :"
                << HttpReqProto ::enum2method( proto.method) << "\t"
                << proto.route <<"\t"
                << http_verenum2str(proto.proto_ver) << "\n";
            if (on_mseeage_cb_ && proto.method != HttpReqProto::Method::INVALIED) {
                on_mseeage_cb_(shared_from_this(),proto);
                sock_.async_read_some(asio::buffer(rd_buf_,rd_buf_.size()),
                std::bind(& Session::do_read,this,std::placeholders::_1,std::placeholders::_2));
            }
            else {
                std::cout << " do read error :"<< ec.message() << "\n";
            }
        }
    }

    void send(HttpResProto res_proto) {
        auto data = res_proto.to_string();
        std::cout << "send data :" << data.size() << "\n";
        send_dgrams_.push({data.size(),data});
        auto& proto = send_dgrams_.front ();
        sock_.async_write_some(asio::buffer(proto.data,proto.rw_sz),
            std::bind(&Session::do_send,this,std::placeholders::_1,std::placeholders::_2));
    }
    void setOnMessageCallback(const OnMessageCallbackType& cb) {
        on_mseeage_cb_ = std::move(cb);
    }
    void setOnCloseCallback(const OnCloseCallbackType& cb) {
        on_close_cb_ = std::move(cb);
    }
private:
    void do_send(const system::error_code& ec,size_t bytes_sent) {
        if (!ec) {
            if (this->send_dgrams_.empty()){
                return; 
            }
            auto& proto = this->send_dgrams_.front();
            proto.rw_sz -= bytes_sent;
            std :: cout << " bytes sent :" << bytes_sent << " proto.rw sz :"
                << proto.rw_sz << "\t" << proto.data.size() << "\n";
                if (proto.rw_sz > 0) {
                    sock_.async_write_some(asio::buffer(proto.data.data() + bytes_sent,proto.rw_sz),
                        std::bind(&Session::do_send,this,std::placeholders::_1,std::placeholders::_2));
                }
                else {
                    this->send_dgrams_.pop();
                }
            }
        else {
            std::cout <<" do send : "<< ec.message () <<"\n";
            if(on_close_cb_) {
                auto sp = shared_from_this();
                on_close_cb_(sp);
            }
        }
    }

private:
    asio::io_context &ioc_;
    asio::ip::tcp::socket sock_;
    std::string uuid_;
    std::vector<char> rd_buf_;
    std::queue<HttpReqProto> recved_dgrams_;
    std::queue<ResProtoNode> send_dgrams_;
    OnMessageCallbackType on_mseeage_cb_;
    OnCloseCallbackType on_close_cb_;
}; 

class AsyncServer : public NonCopyable {
public :
    AsyncServer() = delete;
    AsyncServer(asio::io_context& ioc)
        :ioc_{ioc},
        acceptor_(ioc_) {

    }
    
    AsyncServer& lisent(const std::string& ip,unsigned short port,bool resue = true) {
        auto endpoint = asio::ip::tcp::endpoint {asio::ip::address::from_string(ip),port};
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(resue));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        service_container_.add_service<AppService>();
        return *this;
    }
    void start_accept() {
        auto session_sp = std::make_shared<Session>(ioc_);
        acceptor_.async_accept(session_sp->socket(),
        std::bind(&AsyncServer::do_accept,this,session_sp,std::placeholders::_1));
    }
    void on_message_handler(std::shared_ptr<Session> session,HttpReqProto req_proto) {
        std::cout << "recved data from uuid :"<< session->uuid ()<<" \n ";
        auto res = service_container_.dispatch(req_proto);
        std::cout <<" dispatch end \n";
        if (!res.is_null()){
            std::cout << " res is not null , proto :" << res.content_type() << "\n"; 
            session->send(res.proto());
        }
    }
private:
    void do_accept(std::shared_ptr<Session> session,const system::error_code& ec) {
        auto uuid = uuid_generator_();
        auto uuid_s = uuids::to_string(uuid);
        session->set_uuid(uuid_s);
        session->setOnMessageCallback(
            std::bind(&AsyncServer::on_message_handler,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        ));
        sessions_.insert({uuid_s,session});
        std::cout << "new connnecion comming, uuid :" << uuid_s <<"\n";
        session->start();
        start_accept();
    }
    void close_handler(std::shared_ptr<Session> session) {
        std::cout << "close handler : " << sessions_.size() << "\n";
        if(sessions_.count(session->uuid()) > 0) {
            sessions_.erase(session->uuid());
        }
    }
private:
    uuids::random_generator uuid_generator_;
    asio::io_context& ioc_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::string,std::shared_ptr<Session>> sessions_;
    ServiceContainer service_container_;
};

int main (){
    asio::io_context ioc{};
    AsyncServer server(ioc);
    server.lisent("0.0.0.0",8080)
        .start_accept();
    ioc.run();
    return 0;
}
