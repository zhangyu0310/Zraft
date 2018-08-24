/*
*
*/

#include "znode.h"

bool zraft::Znode::init() {
	Leader* leader = new Leader;
	Follower* follower = new Follower;
	Candidate* candidate = new Candidate;
	machine_.init(follower, nullptr);
	machine_.addStateChange(follower, )
}