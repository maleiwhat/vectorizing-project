#pragma once
#include "PositionGraph_Node.h"
#include "Patch.h"

class PositionGraph
{
public:
	PositionGraph_Nodes	m_AllNodes;
	PositionGraph_Node_ptrs	m_AllNodeptrs;
	PositionGraph_Node_ptrs m_LiveNodes;
	PositionGraph_Node_ptrs m_Joints;
	Lines			m_Lines;
	double_vector2d		m_LinesWidth;
	Lines			m_ContourLines;
	void CheckSize(int s);
	bool AddJoint(const Vector2& joint, const Vector2& p1, const Vector2& p2, const Vector2& p3);
	bool AddNewLine(const Vector2& p1, const Vector2& p2, double width);
	void ComputeJoints();
	PositionGraph(void);
	~PositionGraph(void);
	void MakeGraphLines();	
	void SmoothGraphLines();
	void MakeContourLines();
	Patch MakePatch();
private:
	void InterMakeGraphLines();
	void AddNode(const PositionGraph_Node& pgn);
};

