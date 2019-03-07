/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_RPC_SERVER_H
#define ZRAFT_RPC_SERVER_H

#include <functional>
#include <initializer_list>
#include <map>
#include <unordered_map>
#include <string>

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
using ConnectionMap = std::map<std::string, TcpServer::TcpConnectionPtr>;
using json = nlohmann::json;
using any = linb::any;

class RpcServer {
public:
    using RpcSerCallback = Rpc::RpcSerCallback;
    using RpcCliCallback = Rpc::RpcCliCallback;
    using ConnectCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void()>;
    using RpcMap = std::map<std::string, Rpc>;
    using RpcContextMap = std::unordered_map<std::string, any>;
    using RpcArgument = json;

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
            const RpcSerCallback& cli_send_cb,
            const RpcSerCallback& cli_recv_cb,
            const RpcCliCallback& ser_recv_cb);
    void addRpc(const std::string& rpc_name, const Rpc& rpc);
    void callRpc(
            const std::string& conn,
            const std::string& rpc_name,
            const json& args);
    void setConnectionCallback(const ConnectCallback& cb) {
        conn_cb_ = cb;
    }
    void setDisConnectionCallback(const ConnectCallback& cb) {
        disconn_cb_ = cb;
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
    time_t getNowTime();
    std::string getNowTime2String();
    std::string getConnectionId(const std::string& ip, uint16_t port);
    json makeRpcJson(
            const std::string& rpc_name,
            Rpc::RpcType rpc_type,
            const std::string& conn_id,
            const RpcArgument& args);

    RpcMap rpc_map_;
    std::thread loop_thread_;
    RpcContextMap context_map_;
    // network
    EventLoop loop_;
    TcpServer server_;
    Connector connector_;
    ConnectionMap connections_;
    ConnectCallback conn_cb_;
    ConnectCallback disconn_cb_;
    ErrorCallback error_cb_;
};

#endif //ZRAFT_RPC_SERVER_H