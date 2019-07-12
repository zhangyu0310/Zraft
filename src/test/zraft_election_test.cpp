//
// Created by zhangyu0310 on 19-4-22.
//

#include <zraft.h>

#include <string>

zraft::ZraftOpt dealInput(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    auto opt = dealInput(argc, argv);
    zraft::Zraft raft(opt);
    raft.init();
    raft.start();
    while (1) {
        sleep(10);
    }
}

zraft::ZraftOpt dealInput(int argc, char* argv[]) {
    (void)argc;
    int my_id = std::stoi(argv[1]);
    int id1 = std::stoi(argv[2]);
    int id2 = std::stoi(argv[3]);
    zraft::ZraftOpt opt;
    opt.id_ = static_cast<uint32_t>(my_id);
    opt.port_ = static_cast<uint16_t>(9280 + my_id);
    std::string ip_list1 = "127.0.0.1";
    ip_list1 = ip_list1 + ":" + std::to_string(9280 + id1);
    std::string ip_list2 = "127.0.0.1";
    ip_list2 = ip_list2 + ":" + std::to_string(9280 + id2);
    opt.setMember(ip_list1);
    opt.setMember(ip_list2);
    return opt;
}