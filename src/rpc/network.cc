/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <network.h>

// FIXME: If json key or value has { } the function is error.
bool zraft::network::jsonParse(Buffer* buffer, json* json) {
    auto ptr = buffer->peek();
    int barckets = 0;
    if (ptr[0] != '{') {
        // TODO: Error Message
        return false;
    } else {
        ++barckets;
    }
    size_t i = 1;
    for (; barckets != 0 && i < buffer->readableBytes(); ++i) {
        if (ptr[i] == '{') {
            ++barckets;
        } else if (ptr[i] == '}') {
            --barckets;
        }
    }
    if (barckets == 0) {
        std::string buf(ptr, i);
        try {
            *json = json::parse(buf);
        } catch (json::parse_error& e) {
            // TODO: Error Message
            return false;
        }
        buffer->retrieve(i);
        return true;
    }
    return false;
}

time_t getNowTime() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string getNowTime2String() {
    return std::to_string(getNowTime());
}