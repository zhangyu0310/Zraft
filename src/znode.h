/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZNODE_H
#define ZRAFT_ZNODE_H

#include <chrono>
#include <memory>
#include <set>

#include <bounce/buffer.h>
#include <bounce/connector.h>
#include <bounce/event_loop.h>
#include <bounce/logger.h>
#include <bounce/tcp_server.h>
#include <bounce/tcp_connection.h>

#include <network.h>
#include <zraft_opt.h>
#include <zrole.h>

using Buffer = bounce::Buffer;
using Connector = bounce::Connector;
using EventLoop = bounce::EventLoop;
using TcpServer= bounce::TcpServer;
using TcpConnection = bounce::TcpConnection;

namespace zraft {

class Znode {
public:
    explicit Znode(const ZraftOpt& opt);
    ~Znode() = default;

    void connectOther(const std::string& ip, uint16_t port);

private:
    void ConnectCallback(const TcpServer::TcpConnectionPtr& conn);
    void MessageCallback(
            const TcpServer::TcpConnectionPtr& conn,
            Buffer* buffer, time_t time);
    void WriteCompCallback(const TcpServer::TcpConnectionPtr& conn);
    void ErrorCallback(const TcpServer::TcpConnectionPtr& conn);
    void ConnectErrorCallback(bounce::SockAddress addr, int err);

    // Public info
    time_t start_time_;
    std::chrono::milliseconds hb_duration_;
    uint64_t current_term_;
    int16_t voted_for_;
    uint64_t commit_index_;
    uint64_t last_applied_;
    // For raft role
    std::unique_ptr<Zrole> role_;
    // For network
    EventLoop loop_;
    TcpServer server_;
    Connector connector_;
    std::set<TcpServer::TcpConnectionPtr> connections_;
    // For storage

};

} // namespace zraft

#endif //ZRAFT_ZNODE_H
