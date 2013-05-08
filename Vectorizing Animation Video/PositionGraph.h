#pragma once
#include "PositionGraph_Node.h"
#define CGAL_NO_AUTOLINK
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_set_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2                                   Point_2;
typedef CGAL::Polygon_2<K>                           Polygon_2;
typedef CGAL::Polygon_with_holes_2<K>                Polygon_with_holes_2;
typedef CGAL::Polygon_set_2<K>                       Polygon_set_2;

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
private:
	void InterMakeGraphLines();
	void AddNode(const PositionGraph_Node& pgn);
};

