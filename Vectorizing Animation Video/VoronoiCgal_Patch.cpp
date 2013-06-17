#include <algorithm>
#include "VoronoiCgal_Patch.h"
#include "CgalPatch.h"
#include "CvExtenstion2.h"
#include "curve/HSplineCurve.h"

void VoronoiCgal_Patch::insert_polygon(Delaunay& cdt,
                                       ImageSpline& m_ImageSpline, int idx)
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
	v_prev->info().nesting_level = idx;

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

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
					{
						vh->info().nesting_level = idx;
						cdt.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
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

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
					{
						vh->info().nesting_level = idx;
						cdt.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
				}
			}
		}
	}
}

void VoronoiCgal_Patch::insert_polygonInter2(Delaunay& cdt, ImageSpline& is,
        PatchSpline& ps)
{
	const int NESTING_LEVEL = TRIANGLE_TRANSPARENT;

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
	v_prev->info().nesting_level = NESTING_LEVEL;

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

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
					{
						vh->info().nesting_level = NESTING_LEVEL;
						cdt.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
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

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
					{
						vh->info().nesting_level = NESTING_LEVEL;
						cdt.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
				}
			}
		}
	}
}

void VoronoiCgal_Patch::insert_polygonInter(Delaunay& cdt, ImageSpline& is,
        int idx)
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
		Mesher mesher(m_Delaunay);
		Criteria    criteria(0, 4000);
		mesher.set_criteria(criteria);
		mesher.refine_mesh();
		mark_domains(m_Delaunay);
		LineSegs lineSegs;

		for (auto e = m_Delaunay.finite_edges_begin();
		        e != m_Delaunay.finite_edges_end(); ++e)
		{
			Delaunay::Face_handle fn = e->first->neighbor(e->second);

			//CGAL::Object o = m_Delaunay.dual(e);
			if (!fn->is_in_domain() || !fn->info().in_domain())
			{
				continue;
			}

			if (!m_Delaunay.is_constrained(*e) && (!m_Delaunay.is_infinite(e->first))
			        && (!m_Delaunay.is_infinite(e->first->neighbor(e->second))))
			{
				Delaunay::Segment s = m_Delaunay.geom_traits().construct_segment_2_object()
				                      (m_Delaunay.circumcenter(e->first),
				                       m_Delaunay.circumcenter(e->first->neighbor(e->second)));
				const K::Segment_2* seg = &s;
				Point p1(seg->source().hx(), seg->source().hy());
				Point p2(seg->target().hx(), seg->target().hy());
				Vector2 pp1(p1.hx(), p1.hy());
				Vector2 pp2(p2.hx(), p2.hy());

				if (pp1 == pp2)
				{
					continue;
				}

				lineSegs.push_back(LineSeg(pp1, pp2));
			}
		}

		for (auto it = lineSegs.begin(); it != lineSegs.end(); ++it)
		{
			//m_PositionGraph.AddNewLine(it->beg, it->end, );
		}
	}

	m_PositionGraph.ComputeJoints();
	//printf("joints: %d\n", m_PositionGraph.m_Joints.size());
	//MakeLines();
	MakeGraphLines();
}


void VoronoiCgal_Patch::MakeGraphLines()
{
	m_PositionGraph.MakeGraphLines();
	m_Lines = m_PositionGraph.m_Lines;
	m_Controls.resize(m_Lines.size());

//  for (int i = 0; i < m_Lines.size(); ++i)
//  {
//      m_Controls[i] = GetControlPoint(m_Lines[i], 15);
//  }

	for (int i = 0; i < m_Lines.size(); ++i)
	{
		HSplineCurve hs;
		Line& cps = m_Controls[i];
		Line& res = m_Lines[i];
		Vector2 beg = res.front(), end = res.back();

		for (int j = 0; j < res.size(); ++j)
		{
			hs.AddPointByDistance(res[j]);
		}

		double dis = hs.GetDistance();
		int step = dis / 5.0;
		cps.push_back(beg);

		for (int j = 1; j < step; ++j)
		{
			cps.push_back(hs.GetValue(j * 5));
		}

		cps.push_back(end);
		hs.Clear();

		for (int j = 0; j < cps.size(); ++j)
		{
			hs.AddPointByDistance(cps[j]);
		}

		res.clear();
		res.push_back(beg);

		for (int j = 1; j < dis - 1; ++j)
		{
			res.push_back(hs.GetValue(j));
		}

		res.push_back(end);
	}
}

void VoronoiCgal_Patch::mark_domains(Delaunay& ct, Delaunay::Face_handle start,
                                     int index, std::list<Delaunay::Edge>& border)
{
	if (start->info().nesting_level != TRIANGLE_NOT_INIT)
	{
		return;
	}

	std::list<Delaunay::Face_handle> queue;
	queue.push_back(start);

	while (! queue.empty())
	{
		Delaunay::Face_handle fh = queue.front();
		queue.pop_front();

		if (fh->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			fh->info().nesting_level = index;

			for (int i = 0; i < 3; i++)
			{
				Delaunay::Edge e(fh, i);
				Delaunay::Face_handle n = fh->neighbor(i);

				if (n->info().nesting_level == TRIANGLE_NOT_INIT)
				{
					if (ct.is_constrained(e))
					{
						border.push_back(e);
					}
					else
					{
						queue.push_back(n);
					}
				}
			}
		}
	}
}

void VoronoiCgal_Patch::mark_domains(Delaunay& cdt)
{
	for (Delaunay::All_faces_iterator it = cdt.all_faces_begin();
	        it != cdt.all_faces_end(); ++it)
	{
		it->info().nesting_level = TRIANGLE_NOT_INIT;
	}

	int index = 0;
	std::list<Delaunay::Edge> border;
	Delaunay::Finite_faces_iterator fc = cdt.finite_faces_begin();

	for (; fc != cdt.finite_faces_end(); ++fc)
	{
		int domain = fc->vertex(0)->info().nesting_level;
		domain = fc->vertex(1)->info().nesting_level == 0 ? fc->vertex(
		             1)->info().nesting_level : domain;
		domain = fc->vertex(2)->info().nesting_level == 0 ? fc->vertex(
		             2)->info().nesting_level : domain;

		if (TRIANGLE_NOT_INIT == fc->info().nesting_level && domain == 0)
		{
			mark_domains(cdt, fc, domain, border);
			break;
		}

//      fc->info().nesting_level = TRIANGLE_TRANSPARENT;
//      int constrained = 0;
//
//      for (int i = 0; i < 3; ++i)
//      {
//          Delaunay::Edge e(fc, i);
//
//          if (cdt.is_constrained(e))
//          {
//              constrained++;
//          }
//      }
//
//      if (constrained == 1)
//      {
//          fc->info().nesting_level = 10;
//      }

		if (TRIANGLE_NOT_INIT == fc->info().nesting_level && domain == 0)
		{
			mark_domains(cdt, fc, domain, border);
			break;
		}
	}

	while (! border.empty())
	{
		Delaunay::Edge e = border.front();
		border.pop_front();
		Delaunay::Face_handle n = e.first->neighbor(e.second);
		int domain = e.first->vertex(0)->info().nesting_level >= 0;
		domain += e.first->vertex(1)->info().nesting_level >= 0;
		domain += e.first->vertex(2)->info().nesting_level >= 0;
		int transparent = e.first->vertex(0)->info().nesting_level ==
		                  TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(1)->info().nesting_level == TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(2)->info().nesting_level == TRIANGLE_TRANSPARENT;

		if (transparent >= 2 || domain < 2)
		{
			mark_domains(cdt, n, TRIANGLE_TRANSPARENT, border);
		}
		else if (n->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			//mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			printf("domain: %d\n", domain);
			mark_domains(cdt, n, domain, border);
		}
	}
}
