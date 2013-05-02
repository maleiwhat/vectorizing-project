#include "PositionGraph.h"


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

	for (int i = 0; i < m_LinesWidth.size(); ++i)
	{
		double_vector& cps = m_LinesWidth[i];

		if (cps.size() > 2)
		{
			double_vector addcps;
			double front = cps.front();
			double back = cps.back();
			addcps.push_back(front * 0.4);
			addcps.push_back(front * 0.6);
			addcps.push_back(front * 0.8);
			addcps.push_back(front * 1);
			cps.insert(cps.begin(), addcps.begin(), addcps.end());
			cps.push_back(back * 1);
			cps.push_back(back * 0.8);
			cps.push_back(back * 0.6);
			cps.push_back(back * 0.4);
		}
	}

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
			addcps.push_back(front + frontV * 4);
			addcps.push_back(front + frontV * 3);
			addcps.push_back(front + frontV * 2);
			addcps.push_back(front + frontV * 1);
			cps.insert(cps.begin(), addcps.begin(), addcps.end());
			cps.push_back(back + backV * 1);
			cps.push_back(back + backV * 2);
			cps.push_back(back + backV * 3);
			cps.push_back(back + backV * 4);
		}
	}

	for (int repeatCount = 0; repeatCount < 3; repeatCount++)
	{
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
	}

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
