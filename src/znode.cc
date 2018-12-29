/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <znode.h>

zraft::Znode::Znode(const std::string& ip,
                    uint16_t port,
                    uint32_t thread_num) :
    current_term_(0),
    voted_for_(-1),
    commit_index_(0),
    last_applied_(0),
    server_(&loop_, ip, port, thread_num),
    connector_(&loop_, &server_) {
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
            this, std::placeholders::_1));
    server_.setErrorCallback(std::bind(
            &Znode::ErrorCallback,
            this, std::placeholders::_1));
}

void zraft::Znode::ConnectCallback(const TcpServer::TcpConnectionPtr& conn) {

}

void zraft::Znode::MessageCallback(const TcpServer::TcpConnectionPtr& conn,
        Buffer* buffer, time_t time) {

}

void zraft::Znode::WriteCompCallback(const TcpServer::TcpConnectionPtr& conn) {

}

void zraft::Znode::ErrorCallback(const TcpServer::TcpConnectionPtr& conn) {

}