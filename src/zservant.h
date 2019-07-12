/*
* Copyright (C) 2019 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZSERVANT_H
#define ZRAFT_ZSERVANT_H

#include <zrole.h>

namespace zraft {

class Servant : Zrole {
public:
    void job();
};

} // namespace zraft

#endif //ZRAFT_ZSERVANT_H