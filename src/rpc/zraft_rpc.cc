/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <zraft_rpc.h>

void ZraftRpc::WhoAreYou::to_json(json& j,
        const ZraftRpc::WhoAreYou::Arguments& arg) {
    j = json{{"my_id", arg.my_id},
             {"my_ip", arg.my_ip},
             {"my_port", arg.my_port},
             {"my_start_time", arg.my_start_time},
             {"my_pid", arg.my_pid}};
}

void ZraftRpc::WhoAreYou::from_json(const json& j,
        ZraftRpc::WhoAreYou::Arguments& arg) {
    j.at("my_id").get_to(arg.my_id);
    j.at("my_ip").get_to(arg.my_ip);
    j.at("my_port").get_to(arg.my_port);
    j.at("my_start_time").get_to(arg.my_start_time);
    j.at("my_pid").get_to(arg.my_pid);
}

void ZraftRpc::WhoAreYou::to_json(json& j,
        const ZraftRpc::WhoAreYou::Results& res) {
    j = json{{"my_id", res.my_id},
             {"my_ip", res.my_ip},
             {"my_port", res.my_port},
             {"my_start_time", res.my_start_time},
             {"my_pid", res.my_pid}};
}

void ZraftRpc::WhoAreYou::from_json(const json& j,
        ZraftRpc::WhoAreYou::Results& res) {
    j.at("my_id").get_to(res.my_id);
    j.at("my_ip").get_to(res.my_ip);
    j.at("my_port").get_to(res.my_port);
    j.at("my_start_time").get_to(res.my_start_time);
    j.at("my_pid").get_to(res.my_pid);
}

void ZraftRpc::RequestVote::to_json(json& j,
        const ZraftRpc::RequestVote::Arguments& arg) {
    j = json{{"term", arg.term},
             {"candidateId", arg.candidateId},
             {"lastLogIndex", arg.lastLogIndex},
             {"lastLogTerm", arg.lastLogTerm}};
}

void ZraftRpc::RequestVote::from_json(const json& j,
        ZraftRpc::RequestVote::Arguments& arg) {
    j.at("term").get_to(arg.term);
    j.at("candidateId").get_to(arg.candidateId);
    j.at("lastLogIndex").get_to(arg.lastLogIndex);
    j.at("lastLogTerm").get_to(arg.lastLogTerm);
}

void ZraftRpc::RequestVote::to_json(json& j,
        const ZraftRpc::RequestVote::Results& res) {
    j = json{{"term", res.term},
             {"voteGranted", res.voteGranted}};
}

void ZraftRpc::RequestVote::from_json(const json& j,
        ZraftRpc::RequestVote::Results& res) {
    j.at("term").get_to(res.term);
    j.at("voteGranted").get_to(res.voteGranted);
}