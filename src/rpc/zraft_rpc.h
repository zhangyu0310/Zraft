/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZRAFT_RPC_H
#define ZRAFT_ZRAFT_RPC_H

#include <rpc.h>

namespace ZraftRpc {

namespace WhoAreYou {

struct Arguments {
    uint32_t my_id;
    std::string my_ip;
    uint16_t my_port;
    time_t my_start_time;
    int my_pid;
};
void to_json(json& j, const Arguments& arg);
void from_json(const json& j, Arguments& arg);

struct Results {
    uint32_t my_id;
    std::string my_ip;
    uint16_t my_port;
    time_t my_start_time;
    int my_pid;
};
void to_json(json& j, const Results& res);
void from_json(const json& j, Results& res);

} // namespace WhoAreYou

namespace RequestVote {

struct Arguments {
    uint64_t term;
    uint64_t candidateId;
    uint64_t lastLogIndex;
    uint64_t lastLogTerm;
};
void to_json(json& j, const Arguments& arg);
void from_json(const json& j, Arguments& arg);

struct Results {
    uint64_t term;
    bool voteGranted;
};
void to_json(json& j, const Results& res);
void from_json(const json& j, Results& res);

} // namespace RequestVote

namespace AppendEntries {

struct Arguments {
    uint64_t term;
    uint32_t leaderId;
    uint64_t prevLogTerm;
    uint64_t leaderCommit;
    //key-value pair entries[];
};
void to_json(json& j, const Arguments& arg);
void from_json(const json& j, Arguments& arg);

struct Results {

};
void to_json(json& j, const Results& res);
void from_json(const json& j, Results& res);

} // namespace AppendEntries
} // namespace Zraft

#endif //ZRAFT_ZRAFT_RPC_H
