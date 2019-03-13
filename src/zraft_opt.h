/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZRAFT_OPT_H
#define ZRAFT_ZRAFT_OPT_H

#include <chrono>
#include <string>
#include <vector>

namespace zraft {

//TODO: Can be read by files like ZooKeeper.
struct ZraftOpt {
public:
    ZraftOpt() :
        id_(0),
        local_ip_("127.0.0.1"),
        port_(0),
        thread_num_(0),
        heart_beat_(0),
        log_path_("./log"),
        db_path_("./data") { }
    // format: 255.255.255.255:9999,0.0.0.0:0000
    explicit ZraftOpt(const std::string& ip_list,
            uint32_t id,
            const std::string& local_ip = "127.0.0.1",
            uint16_t local_port = 9281,
            uint16_t thread_num = 0,
            int64_t heart_beat = 0,
            const std::string& log_path = "./log",
            const std::string& db_path = "./data");
    std::vector<std::string> ip_list_;
    uint32_t id_;
    std::string local_ip_;
    uint16_t port_;
    uint16_t thread_num_;
    int64_t heart_beat_;
    std::string log_path_;
    std::string db_path_;

    void setMember(const std::string& ip_port);

private:
    void eraseLocalIpPort();
};

} // namespace Zraft

#endif //ZRAFT_ZRAFT_OPT_H
