#include <algorithm>
#include "VoronoiCgal_Patch.h"
#include "CgalPatch.h"
#include "CvExtenstion2.h"
#include "curve/HSplineCurve.h"

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
		m_Delaunay = Delaunay();
		insert_polygon(m_Delaunay, m_ImageSpline, i);
		insert_polygonInter(m_Delaunay, m_ImageSpline, i);
		LineSegs lineSegs;

		for (auto e = m_Delaunay.finite_edges_begin(); e != m_Delaunay.finite_edges_end(); ++e)
		{
			CGAL::Object o = m_Delaunay.dual(e);

			if (CGAL::object_cast<K::Segment_2>(&o))
			{
				const K::Segment_2* seg = CGAL::object_cast<K::Segment_2>(&o);
				Point p1(seg->source().hx(), seg->source().hy());
				Point p2(seg->target().hx(), seg->target().hy());
				Vector2 pp1(p1.hx(), p1.hy());
				Vector2 pp2(p2.hx(), p2.hy());

				if (pp1 == pp2)
				{
					continue;
				}

				if (m_CgalPatchs[i].CheckInside(p1.hx(), p1.hy()) &&
				                m_CgalPatchs[i].CheckInside(p2.hx(), p2.hy()))
				{
					m_LineSegs.push_back(LineSeg(pp1, pp2));
					lineSegs.push_back(LineSeg(pp1, pp2));
				}
			}
		}

		for (auto it = lineSegs.begin(); it != lineSegs.end(); ++it)
		{
			m_PositionGraph.AddNewLine(it->beg, it->end);
		}

		mark_domains(i);
	}

	m_PositionGraph.ComputeJoints();
	printf("joints: %d\n", m_PositionGraph.m_Joints.size());
	//MakeLines();
	MakeGraphLines();
}


void VoronoiCgal_Patch::MakeGraphLines()
{
	m_PositionGraph.MakeGraphLines();
	m_Lines = m_PositionGraph.m_Lines;
	m_Controls.resize(m_Lines.size());

// 	for (int i = 0; i < m_Lines.size(); ++i)
// 	{
// 		m_Controls[i] = GetControlPoint(m_Lines[i], 15);
// 	}

	for (int i = 0; i < m_Lines.size(); ++i)
	{
		HSplineCurve hs;
		const Line& cps = m_Controls[i];
		Line& res = m_Lines[i];
		Vector2 beg = res.front(), end = res.back();

		for (int j = 0; j < res.size(); ++j)
		{
			hs.AddPointByDistance(res[j]);
		}

		double dis = hs.GetDistance();
		int step = dis / 2.0;
		res.clear();
		res.push_back(beg);
		for (int j = 1; j < step; ++j)
		{
			res.push_back( hs.GetValue(j*2));
		}
		res.push_back(end);
	}
}

bool VoronoiCgal_Patch::mark_domains(int idx, Delaunay::Face_handle start, std::list<Delaunay::Edge>& border, Line& line)
{
	if (start->info().nesting_level != TRIANGLE_NOT_INIT)
	{
		return false;
	}

	bool ret = false;
	std::list<Delaunay::Face_handle> queue;
	queue.push_back(start);
	bool dont_add = false;

	while (! queue.empty())
	{
		Delaunay::Face_handle fh = queue.front();
		queue.pop_front();

		if (fh->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			fh->info().nesting_level = TRIANGLE_TRANSPARENT;
			Point p1 = fh->vertex(0)->point();
			Point p2 = fh->vertex(1)->point();
			Point p3 = fh->vertex(2)->point();
			LineSegs lineSegs;
			int is_insert = 0;

			for (int i = 0; i < 3; i++)
			{
				Delaunay::Edge e(fh, i);
				Delaunay::Face_handle n = fh->neighbor(i);

				if (m_Delaunay.is_infinite(e) || n->info().edge[i] > 0)
				{
					continue;
				}

				fh->info().edge[i] = 1;
				Delaunay::Face_handle fn = e.first->neighbor(e.second);

				for (int ii = 0; ii < 3; ii++)
				{
					Delaunay::Edge ee(n, ii);

					if (e == ee)
					{
						n->info().edge[ii] = 1;
					}
				}

				if (!dont_add && n->info().nesting_level == TRIANGLE_NOT_INIT)
				{
					//queue.push_back(n);
					//border.push_back(e);
				}

				CGAL::Object o = m_Delaunay.dual(e);

				if (CGAL::object_cast<K::Segment_2>(&o))
				{
					const K::Segment_2* seg = CGAL::object_cast<K::Segment_2>(&o);
					Point p1(seg->source().hx(), seg->source().hy());
					Point p2(seg->target().hx(), seg->target().hy());
					Vector2 pp1(p1.hx(), p1.hy());
					Vector2 pp2(p2.hx(), p2.hy());

					if (pp1 == pp2)
					{
						continue;
					}

					if (m_CgalPatchs[idx].CheckInside(p1.hx(), p1.hy()) &&
					                m_CgalPatchs[idx].CheckInside(p2.hx(), p2.hy()))
					{
						queue.push_back(n);
						m_LineSegs.push_back(LineSeg(pp1, pp2));
						lineSegs.push_back(LineSeg(pp1, pp2));
						is_insert++;
					}
				}
			}

			if (is_insert > 0)
			{
				if (is_insert == 3)
				{
					// find joint
					int_vector score(4);
					Vector2s pts;
					pts.push_back(lineSegs.front().beg);
					pts.push_back(lineSegs.front().end);
					score[0] = 1;
					score[1] = 1;

					for (auto it = ++lineSegs.begin(); it != lineSegs.end(); ++it)
					{
						auto findbeg = std::find(pts.begin(), pts.end(), it->beg);

						if (findbeg == pts.end())
						{
							pts.push_back(it->beg);
							score[pts.size() - 1] = 1;
						}
						else
						{
							score[findbeg - pts.begin()] += 1;
						}

						auto findend = std::find(pts.begin(), pts.end(), it->end);

						if (findend == pts.end())
						{
							pts.push_back(it->end);
							score[pts.size() - 1] = 1;
						}
						else
						{
							score[findend - pts.begin()] += 1;
						}
					}

					m_PositionGraph.AddJoint(pts[0], pts[1], pts[2], pts[3]);
				}
				else
				{
					for (auto it = lineSegs.begin(); it != lineSegs.end(); ++it)
					{
						m_PositionGraph.AddNewLine(it->beg, it->end);
					}
				}
			}
		}
	}

	return true;
}
