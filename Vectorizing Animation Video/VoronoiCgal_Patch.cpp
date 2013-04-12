#include <algorithm>
#include "VoronoiCgal_Patch.h"
#include "CgalPatch.h"

void VoronoiCgal_Patch::insert_polygon(Delaunay& cdt, ImageSpline& m_ImageSpline, int idx)
{
	PatchSpline& ps = m_ImageSpline.m_PatchSplines[idx];
	LineIndex start_idx = ps.m_LineIndexs.front();
	Point last;

	if (start_idx.m_Forward)
	{
		Vector2 v = m_ImageSpline.m_LineFragments[start_idx.m_id].m_Points.front();
		last = Point(v.x, v.y);
	}
	else
	{
		Vector2 v = m_ImageSpline.m_LineFragments[start_idx.m_id].m_Points.back();
		last = Point(v.x, v.y);
	}

	Point start = last;
	Delaunay::Vertex_handle v_prev  = cdt.insert(last);

	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = m_ImageSpline.m_LineFragments[it->m_id].m_Points;

		if (it->m_Forward)
		{
			for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
			{
				Point now(it2->x, it2->y);

				if (now != last)
				{
					Delaunay::Vertex_handle vh = cdt.insert(now);
					v_prev = vh;
					last = now;
				}
			}
		}
		else
		{
			for (auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
			{
				Point now(it2->x, it2->y);

				if (now != last)
				{
					Delaunay::Vertex_handle vh = cdt.insert(now);
					v_prev = vh;
					last = now;
				}
			}
		}
	}
}

void VoronoiCgal_Patch::insert_polygonInter2(Delaunay& cdt, ImageSpline& is, PatchSpline& ps)
{
	if (ps.m_LineIndexs.empty())
	{
		return;
	}

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
	Delaunay::Vertex_handle v_prev  = cdt.insert(last);

	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = is.m_LineFragments[it->m_id].m_Points;

		if (it->m_Forward)
		{
			for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
			{
				Point now(it2->x, it2->y);

				if (now != last)
				{
					Delaunay::Vertex_handle vh = m_Delaunay.insert(now);
					v_prev = vh;
					last = now;
				}
			}
		}
		else
		{
			for (auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
			{
				Point now(it2->x, it2->y);

				if (now != last)
				{
					Delaunay::Vertex_handle vh = m_Delaunay.insert(now);
					v_prev = vh;
					last = now;
				}
			}
		}
	}
}

void VoronoiCgal_Patch::insert_polygonInter(Delaunay& cdt, ImageSpline& is, int idx)
{
	PatchSplines& pss = is.m_PatchSplinesInter[idx];

	for (int i = 0; i < pss.size(); ++i)
	{
		insert_polygonInter2(cdt, is, pss[i]);
	}
}

void VoronoiCgal_Patch::Compute()
{
	m_CgalPatchs = MakePatchs(m_ImageSpline);

	for (int i = 0; i < m_CgalPatchs.size(); ++i)
	{
		m_OutLines.push_back(m_CgalPatchs[i].Outer());
		m_OutLines.insert(m_OutLines.end(), m_CgalPatchs[i].Inter().begin(), m_CgalPatchs[i].Inter().end());
	}

	for (int i = 0; i < m_CgalPatchs.size(); ++i)
	{
		m_Delaunay = Delaunay();
		insert_polygon(m_Delaunay, m_ImageSpline, i);
		insert_polygonInter(m_Delaunay, m_ImageSpline, i);
		Delaunay::Finite_edges_iterator eit = m_Delaunay.finite_edges_begin();

		for (; eit != m_Delaunay.finite_edges_end(); ++eit)
		{
			CGAL::Object o = m_Delaunay.dual(eit);

			if (CGAL::object_cast<K::Segment_2>(&o))
			{
				const K::Segment_2* seg = CGAL::object_cast<K::Segment_2>(&o);
				Point p1(seg->source().hx(), seg->source().hy());
				Point p2(seg->target().hx(), seg->target().hy());

				if (m_CgalPatchs[i].CheckInside(p1.hx(), p1.hy()) &&
				                m_CgalPatchs[i].CheckInside(p2.hx(), p2.hy()))
				{
					m_LineSegs.push_back(
					        LineSeg(
					                Vector2(p1.hx(), p1.hy()), Vector2(p2.hx(), p2.hy())
					        )
					);
				}
			}
		}
	}

	//MakeLines();
}

void VoronoiCgal_Patch::MakeLines()
{
	m_Lines.clear();
	Line lastline;
	lastline.push_back(m_LineSegs.front().beg);
	lastline.push_back(m_LineSegs.front().end);
	Vector2 last = m_LineSegs.front().end;
	m_LineSegs.erase(m_LineSegs.begin());

	for (;;)
	{
		for (int i = 0; i < m_LineSegs.size(); ++i)
		{
			if (m_LineSegs[i].beg == last)
			{
				lastline.push_back(m_LineSegs[i].end);
				last = m_LineSegs[i].end;
				m_LineSegs.erase(m_LineSegs.begin() + i);
				i = 0;
			}
			else if (m_LineSegs[i].end == last)
			{
				lastline.push_back(m_LineSegs[i].beg);
				last = m_LineSegs[i].beg;
				m_LineSegs.erase(m_LineSegs.begin() + i);
				i = 0;
			}
		}

		for (int i = 0; i < m_LineSegs.size(); ++i)
		{
			if (m_LineSegs[0].beg == last)
			{
				lastline.push_front(m_LineSegs[0].end);
				last = m_LineSegs[0].end;
				m_LineSegs.erase(m_LineSegs.begin());
				i = 0;
			}
			else if (m_LineSegs[0].end == last)
			{
				lastline.push_front(m_LineSegs[0].beg);
				last = m_LineSegs[0].beg;
				m_LineSegs.erase(m_LineSegs.begin());
				i = 0;
			}
		}

		m_Lines.push_back(lastline);

		if (!m_LineSegs.empty())
		{
			lastline.clear();
			lastline.push_back(m_LineSegs.front().beg);
			lastline.push_back(m_LineSegs.front().end);
			last = m_LineSegs.front().end;
			m_LineSegs.erase(m_LineSegs.begin());
		}
		else
		{
			break;
		}
	}
}
