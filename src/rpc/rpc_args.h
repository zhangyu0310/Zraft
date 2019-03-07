/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_RPC_ARGS_H
#define ZRAFT_RPC_ARGS_H

#include <string>

namespace rpc {

struct Hello {
    int id;
    std::string str;
};

void to_json(json& j, const Hello& hello) {
    j = json{{"id", hello.id}, {"str", hello.str}};
}

void from_json(const json& j, Hello& hello) {
    j.at("id").get_to(hello.id);
    j.at("str").get_to(hello.str);
}

struct HelloRes {
    std::string str;
};

void to_json(json& j, const HelloRes& hello) {
    j = json{{"str", hello.str}};
}

void from_json(const json& j, HelloRes& hello) {
    j.at("str").get_to(hello.str);
}

}

#endif //ZRAFT_RPC_ARGS_H
