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

zraft::Znode::Znode(const zraft::ZraftOpt& opt) :
        current_term_(0),
        voted_for_(-1),
        commit_index_(0),
        last_applied_(0),
        server_(&loop_, opt.local_ip_, opt.port_, opt.thread_num_),
        connector_(&loop_, &server_) {
    start_time_ = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
    if (opt.heart_beat_ < 100) {
        hb_duration_ = std::chrono::milliseconds(100);
    } else {
        hb_duration_ = std::chrono::milliseconds(opt.heart_beat_);
    }
    server_.setConnectionCallback(std::bind(
            &Znode::ConnectCallback,
            this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(
            &Znode::MessageCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    server_.setWriteCompleteCallback(std::bind(
            &Znode::WriteCompCallback,
            this,
            std::placeholders::_1));
    server_.setErrorCallback(std::bind(
            &Znode::ErrorCallback,
            this,
            std::placeholders::_1));
    connector_.setErrorCallback(std::bind(
            &Znode::ConnectErrorCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
}

void zraft::Znode::connectOther(const std::string& ip, uint16_t port) {
    connector_.connect(bounce::SockAddress(ip, port));
}

void zraft::Znode::ConnectCallback(const TcpServer::TcpConnectionPtr& conn) {
    if (conn->state() == TcpConnection::connected) {
        connections_.insert(conn);
    } else if (conn->state() == TcpConnection::disconnected) {
        connections_.erase(conn);
    } else {

    }
}

void zraft::Znode::MessageCallback(const TcpServer::TcpConnectionPtr& conn,
        Buffer* buffer, time_t time) {

}

void zraft::Znode::WriteCompCallback(const TcpServer::TcpConnectionPtr& conn) {

}

void zraft::Znode::ErrorCallback(const TcpServer::TcpConnectionPtr& conn) {

}

void zraft::Znode::ConnectErrorCallback(bounce::SockAddress addr, int err) {

}