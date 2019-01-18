//
// Created by zhangyu on 19-1-17.
//

#include <gtest/gtest.h>
#include <iostream>
#include <network.h>
#include <nlohmann/json.hpp>
#include "../../dep/bounce/bounce/buffer.h"

using json = nlohmann::json;
using Buffer = bounce::Buffer;

TEST(JsonParse, test)
{
    json j;
    //j.push_back("test");
    //j.push_back(1);
    //j.push_back(true);
    json a;
    a["a"] = "aa";
    a["b"] = "bb";
    j["InterfaceName"] = "Init";
    j["Parameter"] = a;
    std::string str = j.dump();
    std::string str2 = str + str;
    Buffer buffer;
    buffer.append(str2);
    json jj;
    zraft::network::jsonParse(&buffer, &jj);
    std::string str1 = jj.dump();
    EXPECT_STREQ(str1.c_str(), str.c_str());
    EXPECT_EQ(buffer.readableBytes(), str.size());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}