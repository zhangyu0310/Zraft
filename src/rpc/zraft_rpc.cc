/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <zraft_rpc.h>

void ZraftRpc::WhoAreYou::to_json(json& j, const Arguments& arg) {
    j = json{{"my_id", arg.my_id},
             {"my_ip", arg.my_ip},
             {"my_port", arg.my_port},
             {"my_start_time", arg.my_start_time},
             {"my_pid", arg.my_pid}};
}

void ZraftRpc::WhoAreYou::from_json(const json& j, Arguments& arg) {
    j.at("my_id").get_to(arg.my_id);
    j.at("my_ip").get_to(arg.my_ip);
    j.at("my_port").get_to(arg.my_port);
    j.at("my_start_time").get_to(arg.my_start_time);
    j.at("my_pid").get_to(arg.my_pid);
}

void ZraftRpc::WhoAreYou::to_json(json& j, const Results& res) {
    j = json{{"my_id", res.my_id},
             {"my_ip", res.my_ip},
             {"my_port", res.my_port},
             {"my_start_time", res.my_start_time},
             {"my_pid", res.my_pid}};
}

void ZraftRpc::WhoAreYou::from_json(const json& j, Results& res) {
    j.at("my_id").get_to(res.my_id);
    j.at("my_ip").get_to(res.my_ip);
    j.at("my_port").get_to(res.my_port);
    j.at("my_start_time").get_to(res.my_start_time);
    j.at("my_pid").get_to(res.my_pid);
}