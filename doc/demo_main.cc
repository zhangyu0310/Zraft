int main() {
    RaftOpt opt(...);
    Raft raft(opt);
    raft.init();
    raft.start(); //选举，心跳

    raft.set(key, value); // 同步,但超时
    raft.get(key);  // 同步，但超时
}
