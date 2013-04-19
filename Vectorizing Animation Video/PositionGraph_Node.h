#pragma once
#include "Line.h"
#include <list>

class PositionGraph_Node;
typedef std::list<PositionGraph_Node> PositionGraph_Nodes;
typedef std::vector<PositionGraph_Node> PositionGraph_Node_vector;
typedef std::vector<PositionGraph_Nodes::iterator> PositionGraph_Node_ptrs;

static const int PositionGraph_JOINT_ID = -99;
static const int PositionGraph_NOT_INIT = -1;

class PositionGraph_Node
{
public:
	Vector2 m_Position;
	PositionGraph_Node_ptrs m_Links;
	int	m_line_id;
	PositionGraph_Node(void);
	~PositionGraph_Node(void);
};


