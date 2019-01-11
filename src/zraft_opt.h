/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZRAFT_OPT_H
#define ZRAFT_ZRAFT_OPT_H

#include <string>
#include <vector>

namespace zraft {

struct ZraftOpt {
public:
    // format: 255.255.255.255:9999,0.0.0.0:0000
    ZraftOpt(const std::string& ip_list,
            const std::string& local_ip = "127.0.0.1",
            uint16_t local_port = 9281,
            uint16_t thread_num = 0,
            const std::string& log_path = "./log",
            const std::string& db_path = "./data");
    std::vector<std::string> ip_list_;
    std::string local_ip_;
    uint16_t port_;
    uint16_t thread_num_;
    std::string log_path_;
    std::string db_path_;
    // heart_beat_;

    void setMember(const std::string& ip_port);
};

} // namespace Zraft

#endif //ZRAFT_ZRAFT_OPT_H
