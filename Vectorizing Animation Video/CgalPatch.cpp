#include "CgalPatch.h"


CgalPatch::CgalPatch(void)
{
}


CgalPatch::~CgalPatch(void)
{
}

bool CgalPatch::CheckInside(double x, double y)
{
	Point pt(x, y);

	if (CGAL::ON_BOUNDED_SIDE == CGAL::bounded_side_2(m_Outer.begin(), m_Outer.end(), pt, K()))
	{
		for (auto it = m_Inter.begin(); it != m_Inter.end(); ++it)
		{
			if (CGAL::ON_BOUNDED_SIDE == CGAL::bounded_side_2(it->begin(), it->end(), pt, K()))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void CgalPatch::insert_polygon(ImageSpline& is, int idx)
{
	PatchSpline& ps = is.m_PatchSplines[idx];
	LineIndex start_idx = ps.m_LineIndexs.front();
	Point last;

	if (start_idx.m_Forward)
	{
		Vector2 v = is.m_LineFragments[start_idx.m_id].m_Points.front();
		last = Point(v.x, v.y);
	}
	else
	{
		Vector2 v = is.m_LineFragments[start_idx.m_id].m_Points.back();
		last = Point(v.x, v.y);
	}

	Point start = last;
	m_Outer.push_back(last);

	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = is.m_LineFragments[it->m_id].m_Points;

		if (it->m_Forward)
		{
			for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
			{
				Point now(it2->x, it2->y);
				m_Outer.push_back(now);
				last = now;
			}
		}
		else
		{
			for (auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
			{
				Point now(it2->x, it2->y);
				m_Outer.push_back(now);
				last = now;
			}
		}
	}

	m_Outer.push_back(start);
}

void CgalPatch::insert_polygonInter2(ImageSpline& is, PatchSpline& ps)
{
	LineIndex start_idx = ps.m_LineIndexs.front();
	Point last;

	if (start_idx.m_Forward)
	{
		Vector2 v = is.m_LineFragments[start_idx.m_id].m_Points.front();
		last = Point(v.x, v.y);
	}
	else
	{
		Vector2 v = is.m_LineFragments[start_idx.m_id].m_Points.back();
		last = Point(v.x, v.y);
	}

	Point start = last;
	m_Inter.push_back(Points());
	Points& points = m_Inter.back();
	points.push_back(last);

	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = is.m_LineFragments[it->m_id].m_Points;

		if (it->m_Forward)
		{
			for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
			{
				Point now(it2->x, it2->y);
				points.push_back(now);
				last = now;
			}
		}
		else
		{
			for (auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
			{
				Point now(it2->x, it2->y);
				points.push_back(now);
				last = now;
			}
		}
	}
}

void CgalPatch::insert_polygonInter(ImageSpline& is, int idx)
{
	PatchSplines& pss = is.m_PatchSplinesInter[idx];

	for (int i = 0; i < pss.size(); ++i)
	{
		insert_polygonInter2(is, pss[i]);
	}
}

void CgalPatch::SetData(ImageSpline& is)
{
	m_Inter.clear();
	m_Outer.clear();

	for (int i = 0; i < is.m_PatchSplines.size(); ++i)
	{
		insert_polygon(is, i);
	}

	for (int i = 0; i < is.m_PatchSplinesInter.size(); ++i)
	{
		insert_polygonInter(is, i);
	}
}


CgalPatchs MakePatchs(ImageSpline& is)
{
	CgalPatchs ans;

	for (int i = 0; i < is.m_PatchSplines.size(); i++)
	{
		ans.push_back(CgalPatch());
		//ans.back().insert_polygon(is, i);
		ans.back().insert_polygonInter(is, i);
	}

	return ans;
}
