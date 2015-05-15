#pragma once
#include <vector>
#include "General.h"

class DrawNode
{
public:
	int		m_Id;
	xRect    m_Pos;
};
typedef std::vector<DrawNode> DrawNodes;
