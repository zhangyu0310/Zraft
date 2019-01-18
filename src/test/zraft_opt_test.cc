//
// Created by zhangyu on 19-1-11.
//

#include <gtest/gtest.h>
#include <iostream>
#include <zraft_opt.h>

TEST(ZraftOpt, init)
{
    zraft::ZraftOpt opt("127.0.0.1:9999,1.7.5.3:1,0.0.0.0:10");
    for (auto& it : opt.ip_list_) {
        std::cout << "[----------] " << it << std::endl;
        std::cout << it.substr(0, it.rfind(':')) << std::endl;
        std::cout << it.substr(it.rfind(':') + 1) << std::endl;
    }
    EXPECT_STREQ(opt.local_ip_.c_str(), "127.0.0.1");
    EXPECT_EQ(opt.port_, 9281);
    EXPECT_EQ(opt.thread_num_, 0);
    EXPECT_STREQ(opt.log_path_.c_str(), "./log");
    EXPECT_STREQ(opt.db_path_.c_str(), "./data");
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}