/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <zraft_opt.h>

zraft::ZraftOpt::ZraftOpt(
        const std::string &ip_list,
        const std::string &local_ip,
        uint16_t local_port,
        uint16_t thread_num,
        const std::string &log_path,
        const std::string &db_path) :
        local_ip_(local_ip),
        port_(local_port),
        thread_num_(thread_num),
        log_path_(log_path),
        db_path_(db_path) {
    std::string::size_type begin = 0;
    std::string::size_type end = ip_list.find(',', begin);
    while (end != std::string::npos) {
        ip_list_.push_back(ip_list.substr(begin, end - begin));
        begin = end + 1;
        end = ip_list.find(',', begin);
    }
    ip_list_.push_back(ip_list.substr(begin));
}
void zraft::ZraftOpt::setMember(const std::string &ip_port) {
    std::string::size_type begin = 0;
    std::string::size_type end = ip_port.find(',', begin);
    while (end != std::string::npos) {
        ip_list_.push_back(ip_port.substr(begin, end - begin));
        begin = end + 1;
        end = ip_port.find(',', begin);
    }
    ip_list_.push_back(ip_port.substr(begin));
}