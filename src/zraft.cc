/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <zraft.h>

zraft::Zraft::Zraft(const zraft::ZraftOpt& opt) : options_(opt) { }

void zraft::Zraft::init() {
    node_.reset(new Znode(options_));
    for (auto& it : options_.ip_list_) {
        auto pos = it.rfind(':');
        node_->connectOther(it.substr(0, pos),
                static_cast<uint16_t>(std::stoi(it.substr(pos + 1))));
    }
}

void zraft::Zraft::start() {
    node_->start(); // election and heartbeats
}