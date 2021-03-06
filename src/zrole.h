/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef ZRAFT_ZROLE_H
#define ZRAFT_ZROLE_H

namespace zraft {

class Zrole {
public:
    virtual void job() = 0;
};

} // namespace zraft

#endif //ZRAFT_ZROLE_H
