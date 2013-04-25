#include "TriangulationCgal_Patch.h"
#include "algSpline2d.h"

void TriangulationCgal_Patch::AddPoint(double x, double y)
{
	m_SeedPoints.push_back(Point(x, y));
}

void TriangulationCgal_Patch::Compute()
{
	m_Triangulation.clear();
	Vertex_handles vhs;

	/* add 4 conner point
	Point lu(-10, -10);
	Point ld(-10, m_h+10);
	Point ru(m_w+10, m_h+10);
	Point rd(m_w+10, -10);
	Triangulation::Vertex_handle v1 = m_Triangulation.insert(lu);
	Triangulation::Vertex_handle v2 = m_Triangulation.insert(ld);
	Triangulation::Vertex_handle v3 = m_Triangulation.insert(ru);
	Triangulation::Vertex_handle v4 = m_Triangulation.insert(rd);
	v1->info().nesting_level = 0;
	v2->info().nesting_level = 0;
	v3->info().nesting_level = 0;
	v4->info().nesting_level = 0;
	m_Triangulation.insert_constraint(v1, v2);
	m_Triangulation.insert_constraint(v2, v3);
	m_Triangulation.insert_constraint(v3, v4);
	m_Triangulation.insert_constraint(v4, v1);
	*/

	if (!m_SeedPoints.empty())
	{
		for (int i = 0; i < m_SeedPoints.size() - 1; ++i)
		{
			m_Triangulation.insert(m_SeedPoints[i]);
		}
	}

	for (int i = 0; i < m_Patch.size(); ++i)
	{
		Polygon poly;

		for (auto it = m_Patch[i].Outer().begin(); it != m_Patch[i].Outer().end(); ++it)
		{
			poly.push_back(Point(it->x, it->y));
		}

		insert_polygon(m_Triangulation, poly, i);

		for (auto it = m_Patch[i].Inter().begin(); it != m_Patch[i].Inter().end(); ++it)
		{
			Polygon poly2;

			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				poly2.push_back(Point(it2->x, it2->y));
			}

			insert_polygon(m_Triangulation, poly2, TRIANGLE_NOT_INIT);
		}
	}

	for (int i = 0; i < m_ImageSpline.m_PatchSplines.size(); ++i)
	{
		insert_polygon(m_Triangulation, m_ImageSpline, i);
	}

	for (int i = 0; i < m_ImageSpline.m_PatchSplinesInter.size(); ++i)
	{
		insert_polygonInter(m_Triangulation, m_ImageSpline, i);
	}

	Mesher mesher(m_Triangulation);
	Criteria	criteria(0, 4000);
	mesher.set_criteria(criteria);
	mesher.refine_mesh();
	mark_domains(m_Triangulation);
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain() && fc->info().in_domain() && fc->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
			t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
			t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);

			if (fc->info().nesting_level >= 0 && m_ColorConstraint.size() > fc->info().nesting_level
			                && m_ColorConstraint[fc->info().nesting_level].get())
			{
				t.m_Colors[0] = m_ColorConstraint[fc->info().nesting_level]->
				                GetColorVector3(t.m_Points[0].x, t.m_Points[0].y);
				t.m_Colors[1] = m_ColorConstraint[fc->info().nesting_level]->
				                GetColorVector3(t.m_Points[1].x, t.m_Points[1].y);
				t.m_Colors[2] = m_ColorConstraint[fc->info().nesting_level]->
				                GetColorVector3(t.m_Points[2].x, t.m_Points[2].y);
			}
			else
			{
				t.m_Colors[0] = 255;
				t.m_Colors[1] = 0;
				t.m_Colors[2] = 0;
			}

			m_Triangles.push_back(t);
		}
	}

	for (auto e = m_Triangulation.finite_edges_begin(); e != m_Triangulation.finite_edges_end(); ++e)
	{
		Triangulation::Face_handle fn = e->first->neighbor(e->second);

		//CGAL::Object o = m_Delaunay.dual(e);
		if (fn->is_in_domain() && fn->info().in_domain() && fn->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			if (!m_Triangulation.is_constrained(*e) && (!m_Triangulation.is_infinite(e->first)) && (!m_Triangulation.is_infinite(e->first->neighbor(e->second))))
			{
				Triangulation::Segment s = m_Triangulation.geom_traits().construct_segment_2_object()
				                           (m_Triangulation.circumcenter(e->first), m_Triangulation.circumcenter(e->first->neighbor(e->second)));
				const K::Segment_2* seg = &s;
				Point p1(seg->source().hx(), seg->source().hy());
				Point p2(seg->target().hx(), seg->target().hy());
				Vector2 pp1(p1.hx(), p1.hy());
				Vector2 pp2(p2.hx(), p2.hy());

				if (pp1 == pp2)
				{
					continue;
				}

				m_LineSegs.push_back(LineSeg(pp1, pp2));
			}
		}
	}

	for (auto it = m_LineSegs.begin(); it != m_LineSegs.end(); ++it)
	{
		m_PositionGraph.AddNewLine(it->beg, it->end);
	}

	m_PositionGraph.ComputeJoints();
	m_PositionGraph.MakeGraphLines();
	printf("joints: %d\n", m_PositionGraph.m_Joints.size());
	m_Lines = m_PositionGraph.m_Lines;
}

void TriangulationCgal_Patch::SetCriteria(float shapebound, float length)
{
	m_Criteria = Criteria(shapebound, length);
}

void TriangulationCgal_Patch::Clear()
{
	m_SeedPoints.clear();
}

void TriangulationCgal_Patch::insert_polygon(Triangulation& cdt, ImageSpline& m_ImageSpline, int idx)
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
	Triangulation::Vertex_handle v_prev  = cdt.insert(last);
	//assert(v_prev->info().nesting_level == -1);
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
					Triangulation::Vertex_handle vh = m_Triangulation.insert(now);

					//assert(vh->info().nesting_level == -1 || vh->info().nesting_level == idx);
					if (vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
					{
						vh->info().nesting_level = idx;
						m_Triangulation.insert_constraint(v_prev, vh);
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
					Triangulation::Vertex_handle vh = m_Triangulation.insert(now);

					//assert(vh->info().nesting_level == -1 || vh->info().nesting_level == idx);
					if (vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
					{
						vh->info().nesting_level = idx;
						m_Triangulation.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
				}
			}
		}
	}

	assert(start == last);
}

void TriangulationCgal_Patch::insert_polygonInter2(Triangulation& cdt, ImageSpline& is, PatchSpline& ps)
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
	Triangulation::Vertex_handle v_prev  = cdt.insert(last);
	//assert(v_prev->info().nesting_level == -1);
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
					Triangulation::Vertex_handle vh = m_Triangulation.insert(now);

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
					{
						vh->info().nesting_level = NESTING_LEVEL;
						m_Triangulation.insert_constraint(v_prev, vh);
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
					Triangulation::Vertex_handle vh = m_Triangulation.insert(now);

					if (vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
					{
						vh->info().nesting_level = NESTING_LEVEL;
						m_Triangulation.insert_constraint(v_prev, vh);
						v_prev = vh;
						last = now;
					}
				}
			}
		}
	}

	assert(start == last);
}

void TriangulationCgal_Patch::insert_polygonInter(Triangulation& cdt, ImageSpline& is, int idx)
{
	PatchSplines& pss = is.m_PatchSplinesInter[idx];

	for (int i = 0; i < pss.size(); ++i)
	{
		insert_polygonInter2(cdt, is, pss[i]);
	}
}

void TriangulationCgal_Patch::insert_polygon(Triangulation& cdt, const Polygon& polygon, int domain)
{
	if (polygon.is_empty()) { return; }

	Triangulation::Vertex_handle v_prev = cdt.insert(*CGAL::cpp0x::prev(polygon.vertices_end()));

	for (Polygon::Vertex_iterator vit = polygon.vertices_begin();
	                vit != polygon.vertices_end(); ++vit)
	{
		Triangulation::Vertex_handle vh = cdt.insert(*vit);
		vh->info().nesting_level = domain;
		cdt.insert_constraint(vh, v_prev);
		v_prev = vh;
	}
}

void	TriangulationCgal_Patch::mark_domains(Triangulation& ct, Triangulation::Face_handle start, int index,
                std::list<Triangulation::Edge>& border)
{
	if (start->info().nesting_level != TRIANGLE_NOT_INIT)
	{
		return;
	}

	std::list<Triangulation::Face_handle> queue;
	queue.push_back(start);

	while (! queue.empty())
	{
		Triangulation::Face_handle fh = queue.front();
		queue.pop_front();

		if (fh->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			fh->info().nesting_level = index;

			for (int i = 0; i < 3; i++)
			{
				Triangulation::Edge e(fh, i);
				Triangulation::Face_handle n = fh->neighbor(i);

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

//explore set of facets connected with non constrained edges,
//and attribute to each such set a nesting level.
//We start from facets incident to the infinite vertex, with a nesting
//level of 0. Then we recursively consider the non-explored facets incident
//to constrained edges bounding the former set and increase the nesting level by 1.
//Facets in the domain are those with an odd nesting level.
void	TriangulationCgal_Patch::mark_domains(Triangulation& cdt)
{
	for (Triangulation::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it)
	{
		it->info().nesting_level = TRIANGLE_NOT_INIT;
	}

	int index = 0;
	std::list<Triangulation::Edge> border;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		int domain = fc->vertex(0)->info().nesting_level;
		domain = fc->vertex(1)->info().nesting_level == 0 ? fc->vertex(1)->info().nesting_level : domain;
		domain = fc->vertex(2)->info().nesting_level == 0 ? fc->vertex(2)->info().nesting_level : domain;

		if (TRIANGLE_NOT_INIT == fc->info().nesting_level && domain == 0)
		{
			mark_domains(cdt, fc, domain, border);
			break;
		}
	}

	while (! border.empty())
	{
		Triangulation::Edge e = border.front();
		border.pop_front();
		Triangulation::Face_handle n = e.first->neighbor(e.second);
		int domain = e.first->vertex(0)->info().nesting_level;
		domain = e.first->vertex(1)->info().nesting_level > domain ? e.first->vertex(1)->info().nesting_level : domain;
		domain = e.first->vertex(2)->info().nesting_level > domain ? e.first->vertex(2)->info().nesting_level : domain;
		int transparent = e.first->vertex(0)->info().nesting_level == TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(1)->info().nesting_level == TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(2)->info().nesting_level == TRIANGLE_TRANSPARENT;

		if (transparent >= 2)
		{
			mark_domains(cdt, n, TRIANGLE_TRANSPARENT, border);
		}
		else if (n->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			//mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			printf("domain: %d\n", domain);

			if (domain != -1)
			{
				mark_domains(cdt, n, domain, border);
			}
		}
	}
}

void TriangulationCgal_Patch::AddPatch(Patch& cvps)
{
	m_Patch.push_back(cvps);
}

void TriangulationCgal_Patch::AddColorConstraint(ColorConstraint_sptr constraint)
{
	m_ColorConstraint.push_back(constraint);
}

void TriangulationCgal_Patch::AddImageSpline(ImageSpline& is)
{
	m_ImageSpline = is;
}

void TriangulationCgal_Patch::SetSize(int w, int h)
{
	m_w = w;
	m_h = h;
}



