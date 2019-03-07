//
// Created by zhangyu0310 on 19-2-27.
//

#include <string>
#include <set>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include <rpc_server.h>
#include <nlohmann/json.hpp>
#include <any/any.hpp>
#include <rpc_args.h>

std::set<std::string> conn_id_set;
std::mutex m;
std::condition_variable cv;

void ser_send_cb(const std::string& name, const json& argv, any* context);
void ser_recv_cb(const std::string& name, const json& argv, any* context);
void cli_recv_cb(const std::string& name, const json& argv, json* send_argv);
void connect_cb(const std::string& conn_id);
void disconnect_cb(const std::string& conn_id);

int main(int argc, char* argv[]) {
    // argv[0] = ./rpc_test
    // local_ip local_port peer_ip peer_port
    std::string local_ip(argv[1]);
    uint16_t local_port = static_cast<uint16_t>(std::stoi(argv[2]));
    //std::string peer_ip(argv[3]);
    //uint16_t peer_port = static_cast<uint16_t>(std::stoi(argv[4]));
    RpcServer rpc_server(local_ip, local_port, 1);
    rpc_server.setConnectionCallback(&connect_cb);
    rpc_server.setDisConnectionCallback(&disconnect_cb);
    rpc_server.registerRpc("hello",
            &ser_send_cb,
            &ser_recv_cb,
            &cli_recv_cb);
    rpc_server.start();
    while (1) {
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [] { return !conn_id_set.empty(); });
        }
        for (auto &it : conn_id_set) {
            rpc_server.callRpc(it, "hello", {{"id", 10}, {"str", "haha"}});
        }
        sleep(1);
    }
}

void ser_send_cb(const std::string& name, const json& argv, any* context) {
    auto hello = argv.get<rpc::Hello>();
    std::cout << "Server Send id: " << hello.id << " Mes: " << hello.str << std::endl;
    any test(std::string("This is a test"));
    context->swap(test);
}

void ser_recv_cb(const std::string& name, const json& argv, any* context) {
    auto hello = argv.get<rpc::HelloRes>();
    std::cout << "Mes: " << hello.str << std::endl;
    any ct;
    ct.swap(*context);
    std::cout << linb::any_cast<std::string>(ct) << std::endl;
}

void cli_recv_cb(const std::string& name, const json& argv, json* send_argv) {
    rpc::Hello hello = argv.get<rpc::Hello>();
    std::cout << "Client Recv id: " << hello.id << " Mes: " << hello.str << std::endl;
    rpc::HelloRes res{"Fuck You"};
    (*send_argv) = res;
}

void connect_cb(const std::string& conn_id) {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Connected... conn_id: " << conn_id << std::endl;
    conn_id_set.insert(conn_id);
    cv.notify_one();
}

void disconnect_cb(const std::string& conn_id) {
    std::cout << "DisConnected... conn_id: " << conn_id << std::endl;
    conn_id_set.erase(conn_id);
}