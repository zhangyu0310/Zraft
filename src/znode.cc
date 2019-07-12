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
#include <define_input.h>

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
        running_(false),
        id_(opt.id_),
        cluster_size_(opt.cluster_size_),
        local_ip_(opt.local_ip_),
        local_port_(opt.port_),
        hb_duration_(0),
        current_term_(0),
        voted_for_(-1),
        commit_index_(0),
        last_applied_(0),
        define_input(),
        timeout_count_(0),
        supporter_num_(0),
        timer_thread_(),
        role_(Servant),
        master_id_(id_),
        gen_(rd_()),
        dis_(0, 50),
        rpc_server_(local_ip_, local_port_, opt.thread_num_),
        logger_(spdlog::basic_logger_mt<spdlog::async_factory>("zraft_log", opt.log_path_)),
        WhoAreYou("WhoAreYou",
                std::bind(&Znode::WAYCliSend, this, _1, _2, _3),
                std::bind(&Znode::WAYCliRecv, this, _1, _2, _3),
                std::bind(&Znode::WAYSerRecv, this, _1, _2, _3)),
        RequestVote("RequestVote",
                std::bind(&Znode::RVCliSend, this, _1, _2, _3),
                std::bind(&Znode::RVCliRecv, this, _1, _2, _3),
                std::bind(&Znode::RVSerRecv, this, _1, _2, _3)) {
    using std::chrono::system_clock;
    start_time_ = system_clock::to_time_t(system_clock::now());
    if (opt.heart_beat_ < 50) {
        hb_duration_ = std::chrono::milliseconds(50);
    } else {
        hb_duration_ = std::chrono::milliseconds(opt.heart_beat_);
    }
    rpc_server_.setConnectionCallback(std::bind(
            &Znode::RpcConnectCallback, this, _1));
    rpc_server_.setDisConnectionCallback(std::bind(
            &Znode::RpcDisConnectCallback, this, _1));
    rpc_server_.addRpc(WhoAreYou.rpc_name(), WhoAreYou);
    rpc_server_.start();
    timer_thread_ = std::thread(&bounce::EventLoop::loop, &timer_loop_);
    logger_->info("Raft heart beat is {}ms.", hb_duration_.count());
    logger_->info("Server ip is {}, listen port is {}", local_ip_, local_port_);
    logger_->info("Start Time {}, Rpc Server is started.", start_time_);
    logger_->flush();
    // FIXME: Debug
    logger_->set_level(spdlog::level::trace);
    logger_->flush_on(spdlog::level::trace);
}

void zraft::Znode::connectOther(const std::string &ip, uint16_t port) {
    rpc_server_.connectOther(ip, port);
}

void zraft::Znode::start() {
    (void)timer_;
    auto random_duration_ = std::chrono::milliseconds(dis_(gen_));
    timer_loop_.runAfter(hb_duration_ + random_duration_,
            std::bind(&Znode::run, this, define_input.j_timeout));
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
    if (it != conn_ids_.end()) {
        auto info = fillConnectionInfo(res);
        std::lock_guard<std::mutex> guard(conn_ids_mutex_);
        it->second = info;
        return true;
    } else {
        logger_->error("update ConnectID failed. conn_id {} can't find.", conn_id);
        return false;
    }
}

void zraft::Znode::checkAndReconnect(
        const ZraftRpc::WhoAreYou::Results& res) {
    int connected = 0;
    for (auto& it : conn_ids_) {
        if ((it.second.id_ == res.my_id) &&
            !connectionPortIsLocalPort(it.first)) {
            ++connected;
        }
    }
    if (connected == 0) {
        rpc_server_.connectOther(res.my_ip, res.my_port);
    } else if (connected > 1) {
        logger_->error("file:{}, line:{}, function:{} "
                       "Connection is more than one.",
                       FILENAME(__FILE__), __LINE__, __FUNCTION__);
    }
}

void zraft::Znode::run(const json& input) {
    std::string type(input["type"].get<std::string>());
    if (role_ == Master) {
        spdlog::info("I am Leader now!!!");
        if (type == "heart beat") {
            spdlog::info("Heart beat!");
            // heart beat.
        }
        auto random_duration_ = std::chrono::milliseconds(dis_(gen_));
        timer_loop_.runAfter(hb_duration_ + random_duration_,
                std::bind(&Znode::run, this, define_input.j_timeout));
    } else if (role_ == Servant) {
        if (type == "timeout") {
            json args;
            ZraftRpc::RequestVote::Arguments arguments
                {current_term_ + 1, id_, commit_index_, current_term_};
            args = arguments;
            for (auto& it : conn_ids_) {
                rpc_server_.callRpc(it.first, "RequestVote", args);
            }
            role_ = Candidate;
            auto random_duration_ = std::chrono::milliseconds(dis_(gen_));
            timer_loop_.runAfter(hb_duration_ + random_duration_,
                    std::bind(&Znode::run, this, define_input.j_timeout));
        } else if (type == "heartbeat") {

        }
    } else if (role_ == Candidate) {
        if (type == "timeout") {

        } else if (type == "election success") {
            role_ = Master;
            timer_loop_.runEvery(hb_duration_,
                    std::bind(&Znode::run, this, define_input.j_heart_beat));
        }
    }
}

void zraft::Znode::WAYCliSend(ConnectionID conn_id, const json& argv, any* context) {
    (void)context;
    logger_->debug("WAYCliSend: ConnectID {}, json dump: {}", conn_id, argv.dump());
}

void zraft::Znode::WAYCliRecv(ConnectionID conn_id, const json& argv, any* context) {
    (void)context;
    auto res = argv.get<ZraftRpc::WhoAreYou::Results>();
    updateConnectIDs(conn_id, res);
    if ((id_ > res.my_id) && connectionPortIsLocalPort(conn_id)) {
        // I am bigger than you! Must I connect you, you can't connect me.
        // So I shutdown the connection, and check if I had connected you.
        // If not, reconnect.
        rpc_server_.shutdown(conn_id);
        checkAndReconnect(res);
    }
}

void zraft::Znode::WAYSerRecv(ConnectionID conn_id, const json& argv, json* send_argv) {
    (void)conn_id;
    using ZraftRpc::WhoAreYou::Results;
    Results res{id_, local_ip_, local_port_, start_time_, getpid()};
    *send_argv = res;
}

void zraft::Znode::RVCliSend(ConnectionID conn_id, const json& argv, any* context) {
    (void)context;
    logger_->debug("RVCliSend: ConnectID {}, json dump: {}", conn_id, argv.dump());
}

void zraft::Znode::RVCliRecv(ConnectionID conn_id, const json& argv, any* context) {
    ZraftRpc::RequestVote::Results res = argv;
    if (res.voteGranted) {
        ++supporter_num_;
        supporters_.insert(conn_id);
    } else {
        supporters_.erase(conn_id);
        if (res.term > current_term_) {
            role_ = Servant;
        }
    }
    if (supporter_num_ > cluster_size_ / 2) {
        timer_loop_.doTaskInThread(
                std::bind(&Znode::run, this,
                        define_input.j_election_success));
    }
}

void zraft::Znode::RVSerRecv(ConnectionID conn_id, const json& argv, json* send_argv) {
    ZraftRpc::RequestVote::Arguments args = argv;
    ZraftRpc::RequestVote::Results res {current_term_, true};
    if (current_term_ > args.term ||
        commit_index_ > args.lastLogIndex) {
        res.voteGranted = false;
    }
    if (res.voteGranted) {
        current_term_ = args.term;
        role_ = Servant;
        master_id_ = args.candidateId;
    }
    res.term = current_term_;
    *send_argv = res;
}