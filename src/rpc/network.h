/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_NETWORK_H
#define ZRAFT_NETWORK_H

#include <bounce/buffer.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Buffer = bounce::Buffer;

namespace zraft {

namespace network{

bool jsonParse(Buffer* buffer, json* json);

} // namespace network

} // namespace zraft

#endif //ZRAFT_NETWORK_H
