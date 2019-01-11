/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <zraft.h>

zraft::Zraft::Zraft(const zraft::ZraftOpt& opt) : options_(opt) { }

int zraft::Zraft::init() {
    node_.reset(new Znode(options_.local_ip_,
            options_.port_,
            options_.thread_num_));
    for (auto& it : options_.ip_list_) {
    }
}