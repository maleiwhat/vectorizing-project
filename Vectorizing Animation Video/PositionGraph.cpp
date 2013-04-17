#include "PositionGraph.h"


PositionGraph::PositionGraph(void)
{
}


PositionGraph::~PositionGraph(void)
{
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
		m_LiveNodes.push_back(m_AllNodeptrs[1]);
		m_LiveNodes.push_back(m_AllNodeptrs[2]);
		m_LiveNodes.push_back(m_AllNodeptrs[3]);
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
			m_LiveNodes.push_back(m_AllNodeptrs.back());
			out->m_Links.push_back(m_AllNodeptrs.back());
		}
		if (idx != 2)
		{
			pgn.m_Position = p2;
			pgn.m_Links.clear();
			pgn.m_Links.push_back(out);
			pv.push_back(pgn);
			AddNode(pgn);
			m_LiveNodes.push_back(m_AllNodeptrs.back());
			out->m_Links.push_back(m_AllNodeptrs.back());
		}
		if (idx != 3)
		{
			pgn.m_Position = p3;
			pgn.m_Links.clear();
			pgn.m_Links.push_back(out);
			pv.push_back(pgn);
			AddNode(pgn);
			m_LiveNodes.push_back(m_AllNodeptrs.back());
			out->m_Links.push_back(m_AllNodeptrs.back());
		}
	}
}

bool PositionGraph::AddNewLine(const Vector2& p1, const Vector2& p2)
{
	if (m_AllNodes.empty())
	{
		PositionGraph_Node n1;
		n1.m_Position = p1;
		PositionGraph_Node n2;
		n2.m_Position = p2;
		AddNode(n1);
		AddNode(n2);
		m_AllNodeptrs[0]->m_Links.push_back(m_AllNodeptrs[1]);
		m_AllNodeptrs[1]->m_Links.push_back(m_AllNodeptrs[0]);
		m_LiveNodes.push_back(m_AllNodeptrs[0]);
		m_LiveNodes.push_back(m_AllNodeptrs[1]);
		return true;
	}
	else
	{
		for (auto it = m_LiveNodes.begin(); it != m_LiveNodes.end(); ++it)
		{
			if ((**it).m_Position == p1 && (**it).m_Links.front()->m_Position != p2)
			{
				PositionGraph_Node n2;
				n2.m_Position = p2;
				n2.m_Links.push_back(*it);
				AddNode(n2);
				(**it).m_Links.push_back(m_AllNodeptrs.back());
				m_LiveNodes.erase(it);
				m_LiveNodes.push_back(m_AllNodeptrs.back());
				return true;
			}
			else if ((**it).m_Position == p2 && (**it).m_Links.front()->m_Position != p1)
			{
				PositionGraph_Node n1;
				n1.m_Position = p1;
				n1.m_Links.push_back(*it);
				AddNode(n1);
				(**it).m_Links.push_back(m_AllNodeptrs.back());
				m_LiveNodes.erase(it);
				m_LiveNodes.push_back(m_AllNodeptrs.back());
				return true;
			}
		}

		return false;
	}
}

void PositionGraph::AddNode( const PositionGraph_Node& pgn )
{
	m_AllNodes.push_back(pgn);
	m_AllNodeptrs.push_back(--m_AllNodes.end());
}
