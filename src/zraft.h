/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZRAFT_H
#define ZRAFT_ZRAFT_H

#include <memory>

#include <zraft_opt.h>
#include <znode.h>


namespace zraft {

class Zraft {
public:
    explicit Zraft(const ZraftOpt& opt);
    ~Zraft() = default;

    int init();

private:
    ZraftOpt options_;
    std::unique_ptr<Znode> node_;
};

} // namespace zraft

#endif //ZRAFT_ZRAFT_H
