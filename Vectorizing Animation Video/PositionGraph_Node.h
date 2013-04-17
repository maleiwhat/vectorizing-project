#pragma once
#include "Line.h"
#include <list>

class PositionGraph_Node;
typedef std::list<PositionGraph_Node> PositionGraph_Nodes;
typedef std::vector<PositionGraph_Node> PositionGraph_Node_vector;
typedef std::vector<PositionGraph_Nodes::iterator> PositionGraph_Node_ptrs;

class PositionGraph_Node
{
public:
	Vector2 m_Position;
	PositionGraph_Node_ptrs m_Links;
	int	m_line_id;
	PositionGraph_Node(void);
	~PositionGraph_Node(void);
};


