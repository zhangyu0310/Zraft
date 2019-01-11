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

#include <zrole.h>

using bounce::Buffer;
using bounce::Connector;
using bounce::EventLoop;
using bounce::TcpServer;
using bounce::TcpConnection;

namespace zraft {

class Znode {
public:
    Znode(const std::string& local_ip,
          uint16_t local_port,
          uint32_t thread_num = 0);
    ~Znode() = default;

private:
    void ConnectCallback(const TcpServer::TcpConnectionPtr& conn);
    void MessageCallback(
            const TcpServer::TcpConnectionPtr& conn,
            Buffer* buffer, time_t time);
    void WriteCompCallback(const TcpServer::TcpConnectionPtr& conn);
    void ErrorCallback(const TcpServer::TcpConnectionPtr& conn);

    // Public info
    time_t start_time_;
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
