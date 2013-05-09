#include "PositionGraph.h"
#include "math\Quaternion.h"

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

#include <boost/polygon/polygon.hpp>
#include <cassert>
namespace gtl = boost::polygon;
using namespace boost::polygon::operators;
typedef gtl::polygon_data<double> gPolygon;
typedef gtl::polygon_traits<gPolygon>::point_type gPoint;
typedef std::vector<gPoint> gPoints;

using namespace gtl; //because of operators
typedef std::vector<gPolygon> PolygonSet;

#include "clipper.hpp"
PositionGraph::PositionGraph(void)
{
}


PositionGraph::~PositionGraph(void)
{
}

bool PositionGraph::AddNewLine(const Vector2& p1, const Vector2& p2, double width)
{
	if (m_AllNodes.empty())
	{
		PositionGraph_Node n1;
		n1.m_Position = p1;
		n1.m_Width = width;
		PositionGraph_Node n2;
		n2.m_Position = p2;
		n2.m_Width = width;
		AddNode(n1);
		AddNode(n2);
		m_AllNodeptrs[0]->m_Links.push_back(m_AllNodeptrs[1]);
		m_AllNodeptrs[1]->m_Links.push_back(m_AllNodeptrs[0]);
		return true;
	}
	else
	{
		for (auto i = 0; i < m_LiveNodes.size(); ++i)
		{
			auto it = m_LiveNodes[i];

			if (it->m_Position == p1 && it->m_Links.front()->m_Position != p2)
			{
				for (auto it2 = m_LiveNodes.begin(); it2 != m_LiveNodes.end(); ++it2)
				{
					if ((**it2).m_Position == p2)
					{
						it->m_Links.push_back(*it2);
						(**it2).m_Links.push_back(it);
						return true;
					}
				}

				PositionGraph_Node n2;
				n2.m_Position = p2;
				n2.m_Width = width;
				n2.m_Links.push_back(it);
				AddNode(n2);
				it->m_Links.push_back(m_AllNodeptrs.back());
				return true;
			}
			else if (it->m_Position == p2 && it->m_Links.front()->m_Position != p1)
			{
				for (auto it2 = m_LiveNodes.begin(); it2 != m_LiveNodes.end(); ++it2)
				{
					if ((**it2).m_Position == p1)
					{
						it->m_Links.push_back(*it2);
						(**it2).m_Links.push_back(it);
						return true;
					}
				}

				PositionGraph_Node n1;
				n1.m_Position = p1;
				n1.m_Width = width;
				n1.m_Links.push_back(it);
				AddNode(n1);
				it->m_Links.push_back(m_AllNodeptrs.back());
				return true;
			}
		}

		PositionGraph_Node n1;
		n1.m_Position = p1;
		n1.m_Width = width;
		PositionGraph_Node n2;
		n2.m_Position = p2;
		n2.m_Width = width;
		AddNode(n1);
		AddNode(n2);
		PositionGraph_Nodes::iterator last_1 = *(m_AllNodeptrs.end() - 1);
		PositionGraph_Nodes::iterator last_2 = *(m_AllNodeptrs.end() - 2);
		last_1->m_Links.push_back(last_2);
		last_2->m_Links.push_back(last_1);
		return false;
	}
}

void PositionGraph::AddNode(const PositionGraph_Node& pgn)
{
	m_AllNodes.push_back(pgn);
	m_AllNodeptrs.push_back(--m_AllNodes.end());
	m_LiveNodes.push_back(m_AllNodeptrs.back());
}

void PositionGraph::ComputeJoints()
{
	m_Joints.clear();

	for (auto it = m_LiveNodes.begin(); it != m_LiveNodes.end(); ++it)
	{
		if ((**it).m_Links.size() > 2 || (**it).m_Links.size() == 1)
		{
			m_Joints.push_back(*it);
		}
	}
}

void PositionGraph::MakeGraphLines()
{
	InterMakeGraphLines();
	bool all_is_ok = false;

	for (; !all_is_ok;)
	{
		all_is_ok = true;

		for (auto it = m_LiveNodes.begin(); it != m_LiveNodes.end(); ++it)
		{
			if ((**it).m_line_id == PositionGraph_NOT_INIT)
			{
				all_is_ok = false;
				m_Joints.clear();
				m_Joints.push_back(*it);
				InterMakeGraphLines();
			}
		}
	}
}

void PositionGraph::CheckSize(int s)
{
	m_AllNodeptrs.reserve(s);
	m_LiveNodes.reserve(s);
}

void PositionGraph::InterMakeGraphLines()
{
	const int PositionGraph_JOINT_ID = -99;
	int now_id = 0;

	for (auto it = m_Joints.begin(); it != m_Joints.end(); ++it)
	{
		(**it).m_line_id = PositionGraph_JOINT_ID;
	}

	for (auto it = m_Joints.begin(); it != m_Joints.end(); ++it)
	{
		for (auto it2 = (**it).m_Links.begin(); it2 != (**it).m_Links.end(); ++it2)
		{
			auto last_it3 = *it;
			auto it3 = *it2;

			// already walk
			if (it3->m_line_id != PositionGraph_NOT_INIT)
			{
				continue;
			}

			double_vector now_width;
			Line now_line;
			now_line.push_back((**it).m_Position);
			now_width.push_back(it3->m_Width);

			for (;;)
			{
				now_line.push_back(it3->m_Position);
				now_width.push_back(it3->m_Width);

				if (it3->m_line_id == PositionGraph_JOINT_ID)
				{
					break;
				}

				// set walked
				if (it3->m_line_id == PositionGraph_NOT_INIT)
				{
					it3->m_line_id = now_id;
				}
				else
				{
					break;
				}

				if (it3->m_Links.size() == 1)
				{
					break;
				}

				bool has_next = false;
				assert(it3->m_Links.size() == 2);

				for (auto it4 = it3->m_Links.begin(); it4 != it3->m_Links.end(); ++it4)
				{
					if ((**it4).m_line_id == PositionGraph_NOT_INIT || ((**it4).m_line_id == PositionGraph_JOINT_ID && *it4 != last_it3))
					{
						last_it3 = it3;
						it3 = *it4;
						has_next = true;
						break;
					}
				}

				assert(has_next);
			}

			if (now_line.size() > 1)
			{
				m_Lines.push_back(now_line);
				m_LinesWidth.push_back(now_width);
			}
		}
	}
}


bool PositionGraph::AddJoint(const Vector2& joint, const Vector2& p1, const Vector2& p2, const Vector2& p3)
{
	if (m_AllNodes.empty())
	{
		PositionGraph_Node j1;
		j1.m_Position = joint;
		PositionGraph_Node n1;
		n1.m_Position = p1;
		PositionGraph_Node n2;
		n2.m_Position = p2;
		PositionGraph_Node n3;
		n3.m_Position = p3;
		AddNode(j1);
		AddNode(n1);
		AddNode(n2);
		AddNode(n3);
		m_AllNodeptrs[0]->m_Links.push_back(m_AllNodeptrs[1]);
		m_AllNodeptrs[0]->m_Links.push_back(m_AllNodeptrs[2]);
		m_AllNodeptrs[0]->m_Links.push_back(m_AllNodeptrs[3]);
		m_AllNodeptrs[1]->m_Links.push_back(m_AllNodeptrs[0]);
		m_AllNodeptrs[2]->m_Links.push_back(m_AllNodeptrs[0]);
		m_AllNodeptrs[3]->m_Links.push_back(m_AllNodeptrs[0]);
		m_Joints.push_back(m_AllNodeptrs[0]);
		return true;
	}
	else
	{
		PositionGraph_Node_vector pv;
		PositionGraph_Node pgn;
		PositionGraph_Nodes::iterator out;
		int idx = 0;

		for (auto it = m_LiveNodes.begin(); it != m_LiveNodes.end(); ++it)
		{
			if ((**it).m_Position == joint)
			{
				if ((**it).m_Links.front()->m_Position == p1)
				{
					idx = 1;
				}
				else if ((**it).m_Links.front()->m_Position == p2)
				{
					idx = 2;
				}
				else if ((**it).m_Links.front()->m_Position == p3)
				{
					idx = 3;
				}

				out = *it;
				m_LiveNodes.erase(it);
				break;
			}
		}

		if (idx == 0)
		{
			return false;
		}

		m_Joints.push_back(out);

		if (idx != 1)
		{
			pgn.m_Position = p1;
			pgn.m_Links.clear();
			pgn.m_Links.push_back(out);
			pv.push_back(pgn);
			AddNode(pgn);
			out->m_Links.push_back(m_AllNodeptrs.back());
		}

		if (idx != 2)
		{
			pgn.m_Position = p2;
			pgn.m_Links.clear();
			pgn.m_Links.push_back(out);
			pv.push_back(pgn);
			AddNode(pgn);
			out->m_Links.push_back(m_AllNodeptrs.back());
		}

		if (idx != 3)
		{
			pgn.m_Position = p3;
			pgn.m_Links.clear();
			pgn.m_Links.push_back(out);
			pv.push_back(pgn);
			AddNode(pgn);
			out->m_Links.push_back(m_AllNodeptrs.back());
		}
	}
}

void PositionGraph::SmoothGraphLines()
{
	// smooth line width
	for (int repeatCount = 0; repeatCount < 3; repeatCount++)
	{
		for (int i = 0; i < m_LinesWidth.size(); ++i)
		{
			double_vector& cps = m_LinesWidth[i];
			double_vector newcps;
			newcps.reserve(cps.size());

			if (cps.size() < 4) { continue; }

			newcps.push_back(cps.front());
			newcps.push_back(*(cps.begin() + 1));

			for (int j = 2; j < cps.size() - 2; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) / 6.0f;
				newcps.push_back(vec);
			}

			newcps.push_back(*(cps.end() - 2));
			newcps.push_back(cps.back());
			cps = newcps;
		}
	}

	// smooth line
	for (int repeatCount = 0; repeatCount < 3; repeatCount++)
	{
		for (int i = 0; i < m_Lines.size(); ++i)
		{
			Line& cps = m_Lines[i];
			Line newcps;

			if (cps.size() < 4) { continue; }

			newcps.push_back(cps.front());

			for (int j = 1; j < cps.size() - 1; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) / 4.0f;
				newcps.push_back(vec);
			}

			newcps.push_back(cps.back());
			cps = newcps;
		}
	}

	// add begin end line width
	for (int i = 0; i < m_LinesWidth.size(); ++i)
	{
		double_vector& cps = m_LinesWidth[i];

		if (cps.size() > 2)
		{
			double_vector addcps;
			double front = cps.front();
			double back = cps.back();
			addcps.push_back(front * 0.6);
			addcps.push_back(front * 0.8);
			addcps.push_back(front * 1);
			cps.insert(cps.begin(), addcps.begin(), addcps.end());
			cps.push_back(back * 1);
			cps.push_back(back * 0.8);
			cps.push_back(back * 0.6);
		}
	}

	// add begin end line
	for (int i = 0; i < m_Lines.size(); ++i)
	{
		Line& cps = m_Lines[i];

		if (cps.size() > 2)
		{
			Line addcps;
			Vector2 frontV = (cps[0] - cps[1]) * 0.5;
			Vector2 backV = (cps[cps.size() - 1] - cps[cps.size() - 2]) * 0.5;
			Vector2 front = cps.front();
			Vector2 back = cps.back();
			addcps.push_back(front + frontV * 3);
			addcps.push_back(front + frontV * 2);
			addcps.push_back(front + frontV * 1);
			cps.insert(cps.begin(), addcps.begin(), addcps.end());
			cps.push_back(back + backV * 1);
			cps.push_back(back + backV * 2);
			cps.push_back(back + backV * 3);
		}
	}

	for (int i = 0; i < m_LinesWidth.size(); ++i)
	{
		double_vector& cps = m_LinesWidth[i];
		double_vector newcps;
		newcps.reserve(cps.size());

		if (cps.size() < 4) { continue; }

		newcps.push_back(cps.front());

		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) / 4.0f;
			newcps.push_back(vec);
		}

		newcps.push_back(cps.back());
		cps = newcps;
	}

	for (int i = 0; i < m_Lines.size(); ++i)
	{
		Line& cps = m_Lines[i];
		Line newcps;

		if (cps.size() < 4) { continue; }

		newcps.push_back(cps.front());

		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) / 4.0f;
			newcps.push_back(vec);
		}

		newcps.push_back(cps.back());
		cps = newcps;
	}
}

void PositionGraph::MakeContourLines()
{
	using namespace ClipperLib;

	for (int i = 0; i < m_Lines.size(); ++i)
	{
		Line now_line = m_Lines[i];
		Line rights;
		m_ContourLines.push_back(Line());
		Line& lineSegs = m_ContourLines.back();
		rights.resize(now_line.size());
		const double_vector& now_linewidth = m_LinesWidth[i];
		rights[0] = Quaternion::GetRotation(now_line[1] - now_line[0], 90);

		for (int j = 1; j < now_line.size(); ++j)
		{
			rights[j] = Quaternion::GetRotation(now_line[j] - now_line[j - 1], 90);
			rights[j].normalise();
		}

		//lineSegs.push_back(now_line.front() + rights.front()*now_linewidth.front() * 0.5);

		for (int j = 0; j < now_line.size(); ++j)
		{
			lineSegs.push_back(now_line[j] + rights[j]*(now_linewidth[j]-0.2) * 0.5);
		}

		//lineSegs.push_back(now_line.back() + rights.back()*now_linewidth.back() * 0.5);

		for (int j = now_line.size() - 1; j >= 0; --j)
		{
			lineSegs.push_back(now_line[j] - rights[j]*(now_linewidth[j]-0.2) * 0.5);
		}
	}

	for (int i = 0; i < m_ContourLines.size(); ++i)
	{
		Line& cps = m_ContourLines[i];
		Line newcps;

		if (cps.size() < 4) { continue; }

		newcps.push_back(cps.front());
		Vector2 last = cps.front();

		for (int j = 1; j < cps.size(); j ++)
		{
			Vector2& now = cps[j];

			if (last.distance(now) > 0.5)
			{
				newcps.push_back(now);
				last = now;
			}
		}

		cps = newcps;
	}

	Clipper c;

	for (int i = 0; i < m_ContourLines.size(); ++i)
	{
		Polygons Qx;
		Qx.resize(1);
		const Line& now_line = m_ContourLines[i];
		Qx[0].resize(now_line.size());

		for (int j = 0; j < now_line.size(); j ++)
		{
			Qx[0][j].X = now_line[j].x * 10000;
			Qx[0][j].Y = now_line[j].y * 10000;
		}

		c.AddPolygons(Qx, ptSubject);
	}

	Polygons sol;
	m_ContourLines.clear();
	c.Execute(ctUnion, sol, pftPositive, pftPositive);

	for (Polygons::size_type i = 0; i < sol.size(); ++i)
	{
		m_ContourLines.push_back(Line());
		Line& cps = m_ContourLines.back();

		for (ClipperLib::Polygon::size_type j = 0; j < sol[i].size(); ++j)
		{
			cps.push_back(Vector2(sol[i][j].X * 0.0001, sol[i][j].Y * 0.0001));
		}
	}

	/*
	PolygonSet ps;

	for (int i = 0; i < m_ContourLines.size(); ++i)
	{
		gPoints Qx;
		const Line& now_line = m_ContourLines[i];

		for (int j = 0; j < now_line.size(); j ++)
		{
			Qx.push_back(gPoint(now_line[j].x, now_line[j].y));
		}

		gPolygon Qxx(Qx.begin(), Qx.end());
		ps += Qxx;
	}

	m_ContourLines.clear();

	for (auto itx = ps.begin(); itx != ps.end(); ++itx)
	{
		m_ContourLines.push_back(Line());
		Line& cps = m_ContourLines.back();

		for (auto vit = itx->begin(); vit != itx->end(); ++vit)
		{
			cps.push_back(now);
		}
	}

	/*
	Polygon_set_2 S;
	Polygon_2 P;

	for (int i = 0; i < m_ContourLines[0].size(); ++i)
	{
		const Line& now_line = m_ContourLines[0];
		P.push_back(Point_2(now_line[i].x, now_line[i].y));
	}

	if (P.is_clockwise_oriented())
	{
		P.reverse_orientation();
	}

	S.insert(P);

	for (int i = 1; i < m_ContourLines.size(); ++i)
	{
		Polygon_2 Q;
		const Line& now_line = m_ContourLines[i];

		for (int j = 0; j < now_line.size(); j ++)
		{
			Q.push_back(Point_2(now_line[j].x, now_line[j].y));
		}

		if (Q.is_simple())
		{
			if (Q.is_clockwise_oriented())
			{
				Q.reverse_orientation();
			}

			S.join(Q);
		}
	}

	std::vector<Polygon_with_holes_2> res;
	std::vector<Polygon_with_holes_2>::const_iterator it;
	S.polygons_with_holes(std::back_inserter(res));
	m_ContourLines.clear();

	for (it = res.begin(); it != res.end(); ++it)
	{
		if (it->outer_boundary().size() > 0)
		{
			m_ContourLines.push_back(Line());
			Line& lineSegs = m_ContourLines.back();

			for (auto vit = it->outer_boundary().vertices_begin(); vit != it->outer_boundary().vertices_end(); ++vit)
			{
				lineSegs.push_back(Vector2(vit->x(), vit->y()));
			}

			lineSegs.push_back(lineSegs.front());
		}

		if (it->has_holes())
		{
			m_ContourLines.push_back(Line());
			Line& lineSegs = m_ContourLines.back();

			for (auto vit = it->holes_begin()->vertices_begin(); vit != it->holes_begin()->vertices_end(); ++vit)
			{
				lineSegs.push_back(Vector2(vit->x(), vit->y()));
			}

			lineSegs.push_back(lineSegs.front());
		}
	}
	*/
}

Patch PositionGraph::MakePatch()
{
	Patch p;

	if (m_ContourLines.size() > 0)
	{
		p.Outer() = m_ContourLines[0];
		p.Inter().insert(p.Inter().begin(),
		                 m_ContourLines.begin() + 1, m_ContourLines.end());
	}

	return p;
}
