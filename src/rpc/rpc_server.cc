/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include "rpc_server.h"

#include <chrono>

using std::chrono::system_clock;

RpcServer::RpcServer(const std::string& ip,
        uint16_t port,
        uint32_t thread_num) :
            g_conn_id_(0),
            loop_(),
            server_(&loop_, ip, port, thread_num),
            connector_(&loop_, &server_),
            logger_(spdlog::basic_logger_mt<spdlog::async_factory>("rpc_log", "./rpc_log")){
    bounce::Logger::setBounceLogPath(".");
    bounce::Logger::get("bounce_file_log")->set_level(spdlog::level::debug);
    bounce::Logger::get("bounce_file_log")->flush_on(spdlog::level::err);
    server_.setConnectionCallback(std::bind(
            &RpcServer::connectCallback,
            this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(
            &RpcServer::messageCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    server_.setWriteCompleteCallback(std::bind(
            &RpcServer::writeCompCallback,
            this,
            std::placeholders::_1));
    server_.setErrorCallback(std::bind(
            &RpcServer::errorCallback,
            this,
            std::placeholders::_1));
    connector_.setErrorCallback(std::bind(
            &RpcServer::connectErrorCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
    server_.start();
    logger_->info("Thread number is {}", thread_num);
    logger_->info("Rpc Server started.");
    logger_->flush();
    // FIXME: For Debug
    logger_->set_level(spdlog::level::err);
    logger_->flush_on(spdlog::level::err);
}

void RpcServer::start() {
    loop_thread_ = std::thread(&bounce::EventLoop::loop, &loop_);
}

void RpcServer::connectOther(const std::string &ip, uint16_t port) {
    bounce::SockAddress addr(ip, port);
    connector_.connect(addr);
    logger_->debug("connect other actively, peer {}:{}", ip, port);
}

void RpcServer::registerRpc(
        const std::string& rpc_name,
        const RpcServer::RpcCliCallback& cli_send_cb,
        const RpcServer::RpcCliCallback& cli_recv_cb,
        const RpcServer::RpcSerCallback& ser_recv_cb) {
    Rpc rpc(rpc_name, cli_send_cb, cli_recv_cb, ser_recv_cb);
    rpc_map_.insert(std::make_pair(rpc_name, rpc));
}

void RpcServer::addRpc(
        const std::string& rpc_name,
        const Rpc& rpc) {
    if (rpc_map_.end() == rpc_map_.find(rpc_name)) {
        rpc_map_.insert(std::make_pair(rpc_name, rpc));
    }
}

void RpcServer::callRpc(
        ConnectionID conn_id,
        const std::string& rpc_name,
        const json& args) {
    logger_->debug("Call RPC: {}", rpc_name);
    auto call_it = rpc_map_.find(rpc_name);
    if (call_it != rpc_map_.end()) {
        auto it = connections_.find(conn_id);
        if (it == connections_.end()) {
            logger_->error("file:{}, line:{}, function:{} "
                           "Can't find conn_id, call RPC failed.",
                           FILENAME(__FILE__), __LINE__, __FUNCTION__);
            if (error_cb_)
                error_cb_();
        } else {
            json rpc_json = makeRpcJson(rpc_name, Rpc::require, conn_id, args);
            auto& conn = it->second;
            conn->send(rpc_json.dump());
            logger_->debug("RPC Server: {}:{}",
                    conn->peer_addr().ip(),
                    conn->peer_addr().port());
            logger_->debug("send mes: {}", rpc_json.dump());
            auto func = call_it->second.cli_send_cb();
            if (func) {
                any context;
                func(conn_id, rpc_json["rpc_args"], &context);
                {
                    std::lock_guard<std::mutex> guard(context_mutex_);
                    context_map_.insert(std::make_pair(
                            rpc_json["rpc_id"].get<std::string>(), context));
                }
            }
        }
    } else {
        logger_->error("file:{}, line:{}, function:{} "
                       "Can't find RPC {}, name mistake or not be registered.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__, rpc_name);
    }
}

void RpcServer::connectCallback(
        const TcpServer::TcpConnectionPtr &conn) {
    if (conn->state() == TcpConnection::connected) {
        ConnectionID conn_id = g_conn_id_++;
        conn->setContext(conn_id);
        {
            std::lock_guard<std::mutex> guard(conn_mutex_);
            connections_.insert(std::make_pair(conn_id, conn));
        }
        conn_cb_(conn_id);
    } else if (conn->state() == TcpConnection::disconnected) {
        auto conn_id = linb::any_cast<ConnectionID>(conn->getContext());
        {
            std::lock_guard<std::mutex> guard(conn_mutex_);
            connections_.erase(conn_id);
        }
        disconn_cb_(conn_id);
    }
}

void RpcServer::messageCallback(
        const TcpServer::TcpConnectionPtr &conn,
        Buffer *buffer,
        time_t time) {
    json mes;
    if (!jsonParse(buffer, &mes)) { //parse failed
        logger_->error("file:{}, line:{}, function:{} "
                       "Json Parse failed.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__);
        return;
    }
    std::string rpc_name(mes["rpc_name"].get<std::string>());
    auto rpc_it = rpc_map_.find(rpc_name);
    if (rpc_it == rpc_map_.end()) {
        logger_->error("file:{}, line:{}, function:{} "
                       "Can't find RPC {}, name mistake or not be registered.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__, rpc_name);
        return;
    }
    auto rpc = rpc_it->second;
    auto type(mes["rpc_type"].get<Rpc::RpcType>());
    auto argv = mes["rpc_args"];
    auto conn_id = linb::any_cast<ConnectionID>(conn->getContext());
    if (type == Rpc::require) {
        auto func = rpc.ser_recv_cb();
        json respond_argv;
        if (func) {
            func(conn_id, argv, &respond_argv);
        }
        json respond = mes;
        respond["rpc_type"] = Rpc::respond;
        respond["rpc_args"] = respond_argv;
        conn->send(respond.dump());
    } else if (type == Rpc::respond) {
        auto func = rpc.cli_recv_cb();
        auto context_it =
                context_map_.find(mes["rpc_id"].get<std::string>());
        any context;
        if (context_it != context_map_.end()) {
            context = context_it->second;
            {
                std::lock_guard<std::mutex> guard(context_mutex_);
                context_map_.erase(context_it);
            }
        }
        if (func) {
            func(conn_id, argv, &context);
        }
    } else {
        logger_->error("file:{}, line:{}, function:{} "
                       "Bad RPC type {}, WTF!",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__, type);
    }
}

void RpcServer::connectErrorCallback(bounce::SockAddress addr, int err) {
    if (error_cb_) {
        error_cb_();
    }
}

void RpcServer::errorCallback(const TcpServer::TcpConnectionPtr &conn) {
    if (error_cb_) {
        error_cb_();
    }
}

void RpcServer::writeCompCallback(
        const TcpServer::TcpConnectionPtr &conn) {
    // TODO: print Message to log.
    auto conn_id = linb::any_cast<ConnectionID>(conn->getContext());
    logger_->info("ConnectID: {}, write completed.", conn_id);
}

// FIXME: If json key or value has { } the function is error.
bool RpcServer::jsonParse(Buffer* buffer, json* json) {
    auto ptr = buffer->peek();
    int barckets = 0;
    if (ptr[0] != '{') {
        logger_->error("jsonParse failed. The first letter is not '{'"
                       "Buffer dump: {}", buffer->peek());
        return false;
    } else {
        ++barckets;
    }
    size_t i = 1;
    for (; barckets != 0 && i < buffer->readableBytes(); ++i) {
        if (ptr[i] == '{') {
            ++barckets;
        } else if (ptr[i] == '}') {
            --barckets;
        }
    }
    if (barckets == 0) {
        std::string buf(ptr, i);
        try {
            *json = json::parse(buf);
        } catch (json::parse_error& e) {
            logger_->error("jsonParse failed. Throw parse error"
                           "Buffer dump: {}", buf);
            return false;
        }
        buffer->retrieve(i);
        return true;
    }
    return false;
}

int64_t RpcServer::getNowTime() {
    return system_clock::now().time_since_epoch().count();
}

std::string RpcServer::getNowTime2String() {
    return std::to_string(getNowTime());
}

json RpcServer::makeRpcJson(
        const std::string& rpc_name,
        Rpc::RpcType rpc_type,
        ConnectionID conn_id,
        const RpcServer::RpcArgument& args) {
    json rpc_json;
    rpc_json["rpc_name"] = rpc_name;
    rpc_json["rpc_type"] = rpc_type;
    rpc_json["rpc_id"] = rpc_name +
            '@' + std::to_string(conn_id) +
            '@' + getNowTime2String();
    rpc_json["rpc_args"] = args;
    return rpc_json;
}