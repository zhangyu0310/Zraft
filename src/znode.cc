/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <znode.h>

#include <unistd.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::placeholders;

static zraft::connect_info fillConnectionInfo(
        const ZraftRpc::WhoAreYou::Results& res) {
    zraft::connect_info info;
    info.id_ = res.my_id;
    info.ip_ = res.my_ip;
    info.port_ = res.my_port;
    info.start_time_ = res.my_start_time;
    info.pid_ = res.my_pid;
    return info;
}

zraft::Znode::Znode(const zraft::ZraftOpt& opt) :
        id_(opt.id_),
        local_ip_(opt.local_ip_),
        local_port_(opt.port_),
        current_term_(0),
        voted_for_(-1),
        commit_index_(0),
        last_applied_(0),
        rpc_server_(local_ip_, local_port_, opt.thread_num_),
        WhoAreYou("WhoAreYou",
                std::bind(&Znode::WAYCliSend, this, _1, _2, _3),
                std::bind(&Znode::WAYCliRecv, this, _1, _2, _3),
                std::bind(&Znode::WAYSerRecv, this, _1, _2, _3)) {
    using std::chrono::system_clock;
    start_time_ = system_clock::to_time_t(system_clock::now());
    if (opt.heart_beat_ < 100) {
        hb_duration_ = std::chrono::milliseconds(100);
    } else {
        hb_duration_ = std::chrono::milliseconds(opt.heart_beat_);
    }
    rpc_server_.setConnectionCallback(std::bind(
            &Znode::RpcConnectCallback, this, _1));
    rpc_server_.setDisConnectionCallback(std::bind(
            &Znode::RpcDisConnectCallback, this, _1));
    rpc_server_.addRpc(WhoAreYou.rpc_name(), WhoAreYou);
    rpc_server_.start();
}

void zraft::Znode::connectOther(const std::string &ip, uint16_t port) {
    rpc_server_.connectOther(ip, port);
}

void zraft::Znode::start() {
    // close useless connections.
    //election();
}

void zraft::Znode::election() {
    for (auto& it : conn_ids_) {
        rpc_server_.callRpc(it.first, "election", {"test", "test"});
    }
}

void zraft::Znode::RpcConnectCallback(ConnectionID conn_id) {
    {
        std::lock_guard<std::mutex> guard(conn_ids_mutex_);
        conn_ids_[conn_id];
    }
    ZraftRpc::WhoAreYou::Arguments arg{id_, local_ip_, local_port_, start_time_, getpid()};
    json j_arg = arg;
    rpc_server_.callRpc(conn_id, "WhoAreYou", j_arg);
}

void zraft::Znode::RpcDisConnectCallback(ConnectionID conn_id) {
    {
        std::lock_guard<std::mutex> guard(conn_ids_mutex_);
        conn_ids_.erase(conn_id);
    }
}

bool zraft::Znode::updateConnectIDs(
        zraft::connect_info::ConnectionID conn_id,
        const ZraftRpc::WhoAreYou::Results& res) {
    auto it = conn_ids_.find(conn_id);
    auto info = fillConnectionInfo(res);
    if (it != conn_ids_.end()) {
        std::lock_guard<std::mutex> guard(conn_ids_mutex_);
        it->second = info;
        return true;
    } else {
        // TODO: Log Error.
        return false;
    }
}

void zraft::Znode::shutdownAndReconnect(
        zraft::connect_info::ConnectionID conn_id,
        const ZraftRpc::WhoAreYou::Results& res) {
    bool connected = false;
    for (auto& it : conn_ids_) {
        if (it.second.id_ == res.my_id) {
            if (!connectionPortIsLocalPort(it.first)) {
                rpc_server_.shutdown(conn_id);
            } else {
                connected = true;
            }
        }
    }
    if (!connected) {
        rpc_server_.connectOther(res.my_ip, res.my_port);
    }
}

void zraft::Znode::WAYCliSend(ConnectionID conn_id, const json& argv, any* context) {
    // TODO: Log...
}

void zraft::Znode::WAYCliRecv(ConnectionID conn_id, const json& argv, any* context) {
    auto res = argv.get<ZraftRpc::WhoAreYou::Results>();
    if (id_ > res.my_id) {
        if (connectionPortIsLocalPort(conn_id)) {
            updateConnectIDs(conn_id, res);
        } else {
            shutdownAndReconnect(conn_id, res);
        }
    } else {
        updateConnectIDs(conn_id, res);
    }
}

void zraft::Znode::WAYSerRecv(ConnectionID conn_id, const json& argv, json* send_argv) {
    using ZraftRpc::WhoAreYou::Results;
    Results res{id_, local_ip_, local_port_, start_time_, getpid()};
    *send_argv = res;
}