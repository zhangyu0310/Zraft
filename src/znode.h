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
#include <set>
#include <queue>
#include <random>

#include <bounce/buffer.h>
#include <bounce/event_loop.h>
#include <bounce/logger.h>

#include <network.h>
#include <zraft_opt.h>
//#include <zrole.h>
#include <rpc.h>
#include <rpc_server.h>
#include <zraft_rpc.h>
#include <define_input.h>

namespace zraft {

struct connect_info {
    using ConnectionID = RpcServer::ConnectionID;
public:
    explicit connect_info() :
        id_(0),
        ip_(),
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
    bool connectionPortIsLocalPort(ConnectionID conn_id) {
        return rpc_server_.getConnectionLocalPort(conn_id) == local_port_;
    }
    bool updateConnectIDs(zraft::connect_info::ConnectionID conn_id,
                          const ZraftRpc::WhoAreYou::Results& res);
    void checkAndReconnect(const ZraftRpc::WhoAreYou::Results &res);
    void run(const json& input);

    // Public info
    std::atomic<bool> running_;
    uint32_t id_;
    uint32_t cluster_size_;
    std::string local_ip_;
    uint16_t local_port_;
    time_t start_time_;
    std::chrono::milliseconds hb_duration_;
    uint64_t current_term_;
    int16_t voted_for_;
    uint64_t commit_index_;
    uint64_t last_applied_;
    DefineInput define_input;

    // Servant use
    std::atomic<int> timeout_count_;

    // Candidate use
    std::atomic<uint16_t> supporter_num_;

    // Master use
    std::set<uint32_t> supporters_;

    // For raft role
    std::thread timer_thread_;
    bounce::EventLoop timer_loop_;
    enum Role { Master, Servant, Candidate };
    Role role_;
    uint32_t master_id_;
    bounce::EventLoop::TimerPtr timer_;
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;

    // For network
    RpcServer rpc_server_;
    std::mutex conn_ids_mutex_;
    std::map<ConnectionID, connect_info> conn_ids_;

    // For log
    std::shared_ptr<spdlog::logger> logger_;

    // For storage

private:
    // RPC Callback Functions.
    // RPC: WhoAreYou
    Rpc WhoAreYou;
    void WAYCliSend(ConnectionID conn_id, const json& argv, any* context);
    void WAYCliRecv(ConnectionID conn_id, const json& argv, any* context);
    void WAYSerRecv(ConnectionID conn_id, const json& argv, json* send_argv);
    // RPC: RequestVote
    Rpc RequestVote;
    void RVCliSend(ConnectionID conn_id, const json& argv, any* context);
    void RVCliRecv(ConnectionID conn_id, const json& argv, any* context);
    void RVSerRecv(ConnectionID conn_id, const json& argv, json* send_argv);
};

} // namespace zraft

#endif //ZRAFT_ZNODE_H
