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

namespace Zraft {
namespace RequestVote {

    struct Arguments {
        uint64_t term;
        uint64_t candidateId;
        uint64_t lastLogIndex;
        uint64_t lastLogTerm;
    };

    struct Results {

    };

    Rpc RequestVote("request_vote", );

} // namespace RequestVote

namespace AppendEntries {
    struct Arguments {
        uint64_t term;
        uint32_t leaderId;
        uint64_t prevLogTerm;
        uint64_t leaderCommit;
        //key-value pair entries[];
    };

    struct Results {

    };

    Rpc AppendEntries("",);

} // namespace AppendEntries
} // namespace Zraft

#endif //ZRAFT_ZRAFT_RPC_H
