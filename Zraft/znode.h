/*
*
*/
#ifndef ZRAFT_ZNODE_H
#define ZRAFT_ZNODE_H
#include <string>
#include <vector>

#include "state.h"
#include "StateMachine/state_machine.hpp"

namespace zraft {

class Znode {
public:
	Znode() {}
	~Znode();
	bool init();

private:
	char voted_for_;
	uint64_t cur_term_;
	std::vector<uint64_t> log_term_;
	uint64_t commit_index_;
	uint64_t last_applied_;
	poppin::CStateMachine<State*, int> machine_;
};

}
#endif // !ZRAFT_ZNODE_H
