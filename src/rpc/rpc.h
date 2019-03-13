/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_RPC_H
#define ZRAFT_RPC_H

#include <functional>
#include <string>

#include <nlohmann/json.hpp>
#include <any/any.hpp>

using json = nlohmann::json;
using any = linb::any;

class Rpc {
public:
    using ConnectionID = uint64_t;
    using RpcCliCallback =
            std::function<void(ConnectionID, const json&, any*)>;
    using RpcSerCallback =
            std::function<void(ConnectionID, const json&, json*)>;
    enum RpcType { require = 0, respond };

public:
    Rpc() = delete;
    Rpc(std::string rpc_name,
        RpcCliCallback cli_send_cb,
        RpcCliCallback cli_recv_cb,
        RpcSerCallback ser_recv_cb) :
            rpc_name_(std::move(rpc_name)),
            cli_send_cb_(std::move(cli_send_cb)),
            cli_recv_cb_(std::move(cli_recv_cb)),
            ser_recv_cb_(std::move(ser_recv_cb)) { }
    ~Rpc() = default;
    Rpc(const Rpc&) = default;
    Rpc& operator=(const Rpc&) = default;

    std::string rpc_name() { return rpc_name_; }
    RpcCliCallback cli_send_cb() { return cli_send_cb_; }
    RpcCliCallback cli_recv_cb() { return cli_recv_cb_; }
    RpcSerCallback ser_recv_cb() { return ser_recv_cb_; }
    void set_cli_send_cb(const RpcCliCallback &cb) {
        cli_send_cb_ = cb;
    }
    void set_cli_recv_cb(const RpcCliCallback &cb) {
        cli_recv_cb_ = cb;
    }
    void set_ser_recv_cb(const RpcSerCallback &cb) {
        ser_recv_cb_ = cb;
    }

private:
    std::string rpc_name_;
    RpcCliCallback cli_send_cb_;
    RpcCliCallback cli_recv_cb_;
    RpcSerCallback ser_recv_cb_;
};

#endif //ZRAFT_RPC_H
