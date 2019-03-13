//
// Created by zhangyu0310 on 19-3-13.
//

#include <string>

#include <zraft.h>

using namespace zraft;

int main(int argc, char* argv[]) {
    int my_id = std::stoi(argv[1]);
    int id1 = std::stoi(argv[2]);
    int id2 = std::stoi(argv[3]);
    ZraftOpt opt;
    opt.id_ = static_cast<uint32_t>(my_id);
    opt.port_ = static_cast<uint16_t>(9280 + my_id);
    std::string ip_list1 = "127.0.0.1";
    ip_list1 = ip_list1 + ":" + std::to_string(9280 + id1);
    std::string ip_list2 = "127.0.0.1";
    ip_list2 = ip_list2 + ":" + std::to_string(9280 + id2);
    opt.setMember(ip_list1);
    opt.setMember(ip_list2);
    Zraft raft(opt);
    raft.init();
    raft.start();
    while (true) {
        sleep(5);
    }
}