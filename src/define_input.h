//
// Created by zhangyu0310 on 19-3-19.
//

#ifndef ZRAFT_DEFINE_INPUT_H
#define ZRAFT_DEFINE_INPUT_H

#include <nlohmann/json.hpp>

namespace zraft {
class DefineInput {
    using json = nlohmann::json;
public:
    DefineInput() :
        j_timeout(R"({
                "type" : "timeout"
            })"_json),
        j_election_success(R"({
                "type" : "election success"
            })"_json),
        j_heart_beat(R"({
                "type" : "heart beat"
            })")
    {}

    json j_timeout;
    json j_election_success;
    json j_heart_beat;
};
}

#endif //ZRAFT_DEFINE_INPUT_H
