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
#include <rpc.h>
#include <rpc_server.h>
#include <zraft_rpc.h>

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

    void connectOther(const std::string& ip,uint16_t port);
    void start();

private:
    void election();
    void RpcConnectCallback(const std::string& conn_id);
    void RpcDisConnectCallback(const std::string& conn_id);

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
    RpcServer rpc_server_;
    std::set<std::string> conn_ids_;
    // For storage

};

} // namespace zraft

#endif //ZRAFT_ZNODE_H
