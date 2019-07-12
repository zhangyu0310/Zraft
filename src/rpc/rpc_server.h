/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_RPC_SERVER_H
#define ZRAFT_RPC_SERVER_H

#define FILENAME(x) strrchr(x, '/')?strrchr(x, '/')+1:x

#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>

#include <bounce/buffer.h>
#include <bounce/connector.h>
#include <bounce/event_loop.h>
#include <bounce/logger.h>
#include <bounce/tcp_server.h>
#include <bounce/tcp_connection.h>
#include <rpc.h>
#include <nlohmann/json.hpp>
#include <any/any.hpp>

using Buffer = bounce::Buffer;
using Connector = bounce::Connector;
using EventLoop = bounce::EventLoop;
using TcpServer= bounce::TcpServer;
using TcpConnection = bounce::TcpConnection;
using json = nlohmann::json;
using any = linb::any;

class RpcServer {
public:
    using ConnectionID = Rpc::ConnectionID;
    using RpcArgument = json;
    using RpcCliCallback = Rpc::RpcCliCallback;
    using RpcSerCallback = Rpc::RpcSerCallback;
    using ConnectCallback = std::function<void(ConnectionID)>;
    using ErrorCallback = std::function<void()>;
    using RpcMap = std::unordered_map<std::string, Rpc>;
    using ConnectionMap = std::unordered_map<ConnectionID, TcpServer::TcpConnectionPtr>;
    using RpcContextMap = std::unordered_map<std::string, any>;

public:
    RpcServer(const std::string& ip,
            uint16_t port,
            uint32_t thread_num);
    ~RpcServer() = default;
    RpcServer(const RpcServer&) = delete;
    RpcServer& operator= (const RpcServer&) = delete;

    void start();
    void connectOther(const std::string& ip, uint16_t port);
    void registerRpc(
            const std::string& rpc_name,
            const RpcCliCallback& cli_send_cb,
            const RpcCliCallback& cli_recv_cb,
            const RpcSerCallback& ser_recv_cb);
    void addRpc(const std::string& rpc_name, const Rpc& rpc);
    void callRpc(
            ConnectionID conn_id,
            const std::string& rpc_name,
            const json& args);
    void setConnectionCallback(const ConnectCallback& cb) {
        conn_cb_ = cb;
    }
    void setDisConnectionCallback(const ConnectCallback& cb) {
        disconn_cb_ = cb;
    }

    void shutdown(ConnectionID conn_id) const {
        auto it = connections_.find(conn_id);
        if (it != connections_.end()) {
            it->second->shutdown();
        } else {
            logger_->error("file:{}, line:{}, function:{} "
                           "Can't find conn_id, shutdown failed.",
                           FILENAME(__FILE__), __LINE__, __FUNCTION__);
        }
    }

    uint16_t getConnectionLocalPort(ConnectionID conn_id) const {
        auto it = connections_.find(conn_id);
        if (it != connections_.end()) {
            return it->second->local_addr().port();
        }
        logger_->error("file:{}, line:{}, function:{} "
                       "Can't find conn_id, get connection local port failed.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__);
        return 0;
    }

    uint16_t getConnectionPeerPort(ConnectionID conn_id) const {
        auto it = connections_.find(conn_id);
        if (it != connections_.end()) {
            return it->second->peer_addr().port();
        }
        logger_->error("file:{}, line:{}, function:{} "
                       "Can't find conn_id, get connection peer port failed.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__);
        return 0;
    }

private:
    void connectCallback(const TcpServer::TcpConnectionPtr& conn);
    void messageCallback(
            const TcpServer::TcpConnectionPtr& conn,
            Buffer* buffer, time_t time);
    void writeCompCallback(const TcpServer::TcpConnectionPtr& conn);
    void errorCallback(const TcpServer::TcpConnectionPtr& conn);
    void connectErrorCallback(bounce::SockAddress addr, int err);
    bool jsonParse(Buffer* buffer, json* json);
    int64_t getNowTime();
    std::string getNowTime2String();
    json makeRpcJson(
            const std::string& rpc_name,
            Rpc::RpcType rpc_type,
            ConnectionID conn_id,
            const RpcArgument& args);

    std::atomic_uint64_t g_conn_id_;
    RpcMap rpc_map_;
    std::thread loop_thread_;
    std::mutex context_mutex_;
    RpcContextMap context_map_;
    // network
    EventLoop loop_;
    TcpServer server_;
    Connector connector_;
    std::mutex conn_mutex_;
    ConnectionMap connections_;
    ConnectCallback conn_cb_;
    ConnectCallback disconn_cb_;
    ErrorCallback error_cb_;
    // logger
    std::shared_ptr<spdlog::logger> logger_;
};

#endif //ZRAFT_RPC_SERVER_H