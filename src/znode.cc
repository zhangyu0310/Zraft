/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <znode.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::placeholders;

zraft::Znode::Znode(const zraft::ZraftOpt& opt) :
        current_term_(0),
        voted_for_(-1),
        commit_index_(0),
        last_applied_(0),
        rpc_server_(opt.local_ip_, opt.port_, opt.thread_num_) {
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
            &Znode::RpcDissConnectCallback, this, _1));
}

void zraft::Znode::connectOther(const std::string &ip, uint16_t port) {
    rpc_server_.connectOther(ip, port);
}

void zraft::Znode::start() {
    // close useless connections.
    election();
}

void zraft::Znode::election() {
    for (auto& it : conn_ids_) {
        rpc_server_.callRpc(it, "election", {"test", "test"});
    }
}

void zraft::Znode::RpcConnectCallback(const std::string& conn_id) {
    conn_ids_.insert(conn_id);
}

void zraft::Znode::RpcDisConnectCallback(const std::string &conn_id) {
    conn_ids_.erase(conn_id);
}