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
#include <mutex>
#include <map>

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

namespace zraft {

struct connect_info {
    using ConnectionID = RpcServer::ConnectionID;
public:
    explicit connect_info() :
        id_(0),
        port_(0),
        start_time_(0),
        pid_(0) { }

    uint32_t id_;
    std::string ip_;
    uint16_t port_;
    time_t start_time_;
    int pid_;
};

class Znode {
    using ConnectionID = RpcServer::ConnectionID;
public:
    explicit Znode(const ZraftOpt& opt);
    ~Znode() = default;

    void connectOther(const std::string& ip,uint16_t port);
    void start();

private:
    void RpcConnectCallback(ConnectionID conn_id);
    void RpcDisConnectCallback(ConnectionID conn_id);
    void election();
    bool connectionPortIsLocalPort(ConnectionID conn_id) {
        return rpc_server_.getConnectionLocalPort(conn_id) == local_port_;
    }
    bool updateConnectIDs(zraft::connect_info::ConnectionID conn_id,
                          const ZraftRpc::WhoAreYou::Results& res);
    void shutdownAndReconnect(zraft::connect_info::ConnectionID conn_id,
                              const ZraftRpc::WhoAreYou::Results& res);
    // Public info
    uint32_t id_;
    std::string local_ip_;
    uint16_t local_port_;
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
    std::mutex conn_ids_mutex_;
    std::map<ConnectionID, connect_info> conn_ids_;
    // For storage

private:
    // RPC Callback Functions.
    // RPC: WhoAreYou
    Rpc WhoAreYou;
    void WAYCliSend(ConnectionID conn_id, const json& argv, any* context);
    void WAYCliRecv(ConnectionID conn_id, const json& argv, any* context);
    void WAYSerRecv(ConnectionID conn_id, const json& argv, json* send_argv);
};

} // namespace zraft

#endif //ZRAFT_ZNODE_H
