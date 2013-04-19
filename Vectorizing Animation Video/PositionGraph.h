#pragma once
#include "PositionGraph_Node.h"

class PositionGraph
{
public:
	PositionGraph_Nodes	m_AllNodes;
	PositionGraph_Node_ptrs	m_AllNodeptrs;
	PositionGraph_Node_ptrs m_LiveNodes;
	PositionGraph_Node_ptrs m_Joints;
	Lines			m_Lines;
	bool AddJoint(const Vector2& joint, const Vector2& p1, const Vector2& p2, const Vector2& p3);
	bool AddNewLine(const Vector2& p1, const Vector2& p2);
	void ComputeJoints();
	PositionGraph(void);
	~PositionGraph(void);
	void MakeGraphLines()
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
				Line now_line;
				now_line.push_back((**it).m_Position);
				auto last_it3 = *it;
				auto it3 = *it2;

				// already walk
				if (it3->m_line_id != PositionGraph_NOT_INIT)
				{
					continue;
				}

				for (;;)
				{
					now_line.push_back(it3->m_Position);

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
				}
			}
		}
	}
	
private:
	void AddNode(const PositionGraph_Node& pgn);
};

