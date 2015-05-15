#pragma once
#include <vector>
#include "shared_ptr.h"

class bnode;
SHARE_PTR(bnode);

class bnode
{
public:
	int id;
	std::vector<int> childs;
};


class btree
{
public:
	btree(void);
	~btree(void);
	bnode_vector m_nodes;
};

