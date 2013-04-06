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
	CgalPatchs cgalpatchs = MakePatchs(m_ImageSpline);
	for (int i = 0; i < m_ImageSpline.m_PatchSplines.size(); ++i)
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

				if (cgalpatchs[i].CheckInside(p1.hx(), p1.hy()) &&
					cgalpatchs[i].CheckInside(p2.hx(), p2.hy()))
				{
					m_Lines.push_back(
						LineSeg(
						Vector2(p1.hx(), p1.hy()), Vector2(p2.hx(), p2.hy())
						)
						);
				}
			}
		}
	}
}
