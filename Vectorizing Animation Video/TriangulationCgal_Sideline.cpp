#include "TriangulationCgal_Sideline.h"
#include "algSpline2d.h"
#include "math/Quaternion.h"
#include "curve/HSplineCurve.h"

void TriangulationCgal_Sideline::AddPoint(double x, double y)
{
	m_SeedPoints.push_back(Point(x, y));
}

int TriangulationCgal_Sideline::Compute()
{
	m_Triangulation.clear();
	Vertex_handles vhs;
	/* add 4 conner point
	Point lu(0, 0);
	Point ld(0, m_h);
	Point ru(m_w, m_h);
	Point rd(m_w, 0);
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
	m_LineSegs.clear();
	for (int i = 0; i < m_ImageSpline.m_PatchSplines.size(); ++i)
	{
		insert_polygon(m_Triangulation, m_ImageSpline, i);
	}
	for (int i = 0; i < m_ImageSpline.m_PatchSplinesInter.size(); ++i)
	{
		insert_polygonInter(m_Triangulation, m_ImageSpline, i);
	}
	Mesher mesher(m_Triangulation);
	mesher.set_criteria(m_Criteria);
	mesher.mark_facets();
	mark_domains(m_Triangulation);
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	LineSegs lineSegs;
	double_vector   LinesWidth;
	for (fc = m_Triangulation.finite_faces_begin();
			fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain() && fc->info().in_domain()
				&& fc->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			Vector2s pts;
			int constrained = 0;
			double_vector widths;
			for (int i = 0; i < 3; i++)
			{
				Triangulation::Edge e(fc, i);
				if (!m_Triangulation.is_constrained(e))
				{
					Vector2 e1(e.first->vertex(m_Triangulation.ccw(e.second))->point().hx(),
							   e.first->vertex(m_Triangulation.ccw(e.second))->point().hy());
					Vector2 e2(e.first->vertex(m_Triangulation.cw(e.second))->point().hx(),
							   e.first->vertex(m_Triangulation.cw(e.second))->point().hy());
					pts.push_back(e1.midPoint(e2));
					widths.push_back(e1.distance(e2));
					++constrained;
				}
			}
			// detect line
			if (constrained == 2)
			{
				LinesWidth.push_back((widths[0] + widths[1]) * 0.5);
				lineSegs.push_back(LineSeg(pts[0], pts[1]));
				m_LineSegs.push_back(LineSeg(pts[0], pts[1]));
			}
			// detect joint
			else if (constrained == 3)
			{
				Vector2 mid = (pts[0] + pts[1] + pts[2]) / 3;
				LinesWidth.insert(LinesWidth.end(), widths.begin(), widths.end());
				lineSegs.push_back(LineSeg(pts[0], mid));
				lineSegs.push_back(LineSeg(pts[1], mid));
				lineSegs.push_back(LineSeg(pts[2], mid));
				m_LineSegs.push_back(LineSeg(pts[0], mid));
				m_LineSegs.push_back(LineSeg(pts[1], mid));
				m_LineSegs.push_back(LineSeg(pts[2], mid));
			}
		}
	}
	int i = 0;
	for (auto it = lineSegs.begin(); it != lineSegs.end(); ++it, ++i)
	{
		m_PositionGraph.AddNewLine(it->beg, it->end, 1);
	}
	m_PositionGraph.ComputeJoints();
	m_PositionGraph.MakeGraphLines();
	m_OriginLines = m_PositionGraph.m_Lines;
	m_PositionGraph.SmoothGraphLines();
	m_Lines = m_PositionGraph.m_Lines;
	m_PositionGraph.MakeContourLines();
	m_ContourLines = m_PositionGraph.m_ContourLines;
	m_LinesWidth = m_PositionGraph.m_LinesWidth;
	m_Controls.resize(m_Lines.size());
	m_Patch.push_back(m_PositionGraph.MakePatch());
	for (int i = 0; i < m_Lines.size(); ++i)
	{
		HSplineCurve hs;
		Line& cps = m_Controls[i];
		Line& res = m_Lines[i];
// 		Vector2 beg = res.front(), end = res.back();
// 		for (int j = 0; j < res.size(); ++j)
// 		{
// 			hs.AddPointByDistance(res[j]);
// 		}
// 		double dis = hs.GetDistance();
// 		int step = dis / 3.0;
// 		cps.push_back(beg);
// 		for (int j = 1; j < step; ++j)
// 		{
// 			cps.push_back(hs.GetValue(j * 3));
// 		}
// 		cps.push_back(end);
// 		hs.Clear();
// 		for (int j = 0; j < cps.size(); ++j)
// 		{
// 			hs.AddPointByDistance(cps[j]);
// 		}
// 		res.clear();
// 		res.push_back(beg);
// 		for (int j = 1; j < dis - 1; ++j)
// 		{
// 			res.push_back(hs.GetValue(j));
// 		}
// 		res.push_back(end);
	}
	m_Triangulation.clear();
	Point lu(0, 0);
	Point ld(0, m_h);
	Point ru(m_w, m_h);
	Point rd(m_w, 0);
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
			//insert_polygon(m_Triangulation, poly2, TRIANGLE_TRANSPARENT);
			insert_polygon(m_Triangulation, poly2, TRIANGLE_NOT_INIT);
		}
	}
	mesher.mark_facets();
	return mark_domains2(m_Triangulation);
}

void TriangulationCgal_Sideline::SetCriteria(float shapebound, float length)
{
	m_Criteria = Criteria(shapebound, length);
}

void TriangulationCgal_Sideline::Clear()
{
	m_SeedPoints.clear();
}

void TriangulationCgal_Sideline::insert_polygon(Triangulation& cdt,
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
	Triangulation::Vertex_handle v_prev  = cdt.insert(last);
	//assert(v_prev->info().nesting_level == -1);
	if (v_prev->info().nesting_level == -1)
	{
		v_prev->info().nesting_level = idx;
	}
	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = m_ImageSpline.m_LineFragments[it->m_id].m_Points;
		// show normal
//      for (int j = 1; j < pts.size(); ++j)
//      {
//          Vector2 right = Quaternion::GetRotation(pts[j] - pts[j - 1], 90);
//          m_LineSegs.push_back(LineSeg(pts[j], pts[j] + right*3));
//      }
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
	//assert(start == last);
	if (start != last)
	{
		m_Triangulation.insert_constraint(last, start);
	}
}

void TriangulationCgal_Sideline::insert_polygonInter2(Triangulation& cdt,
		ImageSpline& is, PatchSpline& ps)
{
	const int NESTING_LEVEL = TRIANGLE_TRANSPARENT;
	if (ps.m_LineIndexs.empty())
	{
		return;
	}
	LineIndex start_idx = ps.m_LineIndexs.front();
	Point last;
	if (is.m_LineFragments[start_idx.m_id].m_Points.size() < 4)
	{
		return;
	}
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
						if (abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
						{
							m_Triangulation.insert_constraint(v_prev, vh);
						}
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
						if (abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
						{
							m_Triangulation.insert_constraint(v_prev, vh);
						}
						v_prev = vh;
						last = now;
					}
				}
			}
		}
	}
	//assert(start == last);
	if (start != last)
	{
		m_Triangulation.insert_constraint(last, start);
	}
}

void TriangulationCgal_Sideline::insert_polygonInter(Triangulation& cdt,
		ImageSpline& is, int idx)
{
	PatchSplines& pss = is.m_PatchSplinesInter[idx];
	for (int i = 0; i < pss.size(); ++i)
	{
		insert_polygonInter2(cdt, is, pss[i]);
	}
}

void TriangulationCgal_Sideline::insert_polygon(Triangulation& cdt,
		const Polygon& polygon, int domain)
{
	if (polygon.is_empty())
	{
		return;
	}
	Triangulation::Vertex_handle v_prev = cdt.insert(*CGAL::cpp0x::prev(
			polygon.vertices_end()));
	for (Polygon::Vertex_iterator vit = polygon.vertices_begin();
			vit != polygon.vertices_end(); ++vit)
	{
		Triangulation::Vertex_handle vh = cdt.insert(*vit);
		vh->info().nesting_level = domain;
		cdt.insert_constraint(vh, v_prev);
		v_prev = vh;
	}
}

//explore set of facets connected with non constrained edges,
//and attribute to each such set a nesting level.
//We start from facets incident to the infinite vertex, with a nesting
//level of 0. Then we recursively consider the non-explored facets incident
//to constrained edges bounding the former set and increase the nesting level by 1.
//Facets in the domain are those with an odd nesting level.
void    TriangulationCgal_Sideline::mark_domains(Triangulation& ct,
		Triangulation::Face_handle start, int index,
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

void    TriangulationCgal_Sideline::mark_domains(Triangulation& cdt)
{
	for (Triangulation::All_faces_iterator it = cdt.all_faces_begin();
			it != cdt.all_faces_end(); ++it)
	{
		it->info().nesting_level = TRIANGLE_NOT_INIT;
	}
	int index = 0;
	std::list<Triangulation::Edge> border;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
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
	}
	while (! border.empty())
	{
		Triangulation::Edge e = border.front();
		border.pop_front();
		Triangulation::Face_handle n = e.first->neighbor(e.second);
		int domain = e.first->vertex(0)->info().nesting_level;
		domain = e.first->vertex(1)->info().nesting_level > domain ? e.first->vertex(
					 1)->info().nesting_level : domain;
		domain = e.first->vertex(2)->info().nesting_level > domain ? e.first->vertex(
					 2)->info().nesting_level : domain;
		int transparent = e.first->vertex(0)->info().nesting_level ==
						  TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(1)->info().nesting_level == TRIANGLE_TRANSPARENT;
		transparent += e.first->vertex(2)->info().nesting_level == TRIANGLE_TRANSPARENT;
		if (transparent >= 2)
		{
			mark_domains(cdt, n, TRIANGLE_TRANSPARENT, border);
		}
		else if (n->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			//mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			//printf("domain: %d\n", domain);
			if (domain != -1)
			{
				mark_domains(cdt, n, domain, border);
			}
		}
	}
}

void TriangulationCgal_Sideline::AddPatch(Patch& cvps)
{
	m_Patch.push_back(cvps);
}

void TriangulationCgal_Sideline::AddColorConstraint(ColorConstraint_sptr
		constraint)
{
	m_ColorConstraint.push_back(constraint);
}

void TriangulationCgal_Sideline::AddImageSpline(ImageSpline& is)
{
	m_ImageSpline = is;
}

void TriangulationCgal_Sideline::SetSize(int w, int h)
{
	m_w = w;
	m_h = h;
}


void    TriangulationCgal_Sideline::mark_domains2(Triangulation& ct,
		Triangulation::Face_handle start, int index,
		std::list<Triangulation::Edge>& border, int color_label)
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
			fh->info().color_label = color_label;
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

int    TriangulationCgal_Sideline::mark_domains2(Triangulation& cdt)
{
	for (Triangulation::All_faces_iterator it = cdt.all_faces_begin();
			it != cdt.all_faces_end(); ++it)
	{
		it->info().nesting_level = TRIANGLE_NOT_INIT;
	}
	int cc = 0;
	std::list<Triangulation::Edge> border;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (TRIANGLE_NOT_INIT == fc->info().nesting_level)
		{
			mark_domains2(cdt, fc, 0, border, cc++);
			break;
		}
	}
	while (! border.empty())
	{
		Triangulation::Edge e = border.front();
		border.pop_front();
		Triangulation::Face_handle n = e.first->neighbor(e.second);
		if (n->info().nesting_level == TRIANGLE_NOT_INIT)
		{
			mark_domains2(cdt, n, e.first->info().nesting_level + 1, border, cc++);
		}
	}
	return cc;
}

void TriangulationCgal_Sideline::MakeColorSequential()
{
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain() && fc->info().in_domain()
				&& fc->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
			t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
			t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
			int label = fc->info().color_label;
			Vector3 vm;
			vm.x = label % 255;
			vm.y = (label / 255) % 255;
			vm.z = label / 255 / 255;
			t.m_Colors[0] = vm;
			t.m_Colors[1] = vm;
			t.m_Colors[2] = vm;
			m_Triangles.push_back(t);
		}
	}
}

void TriangulationCgal_Sideline::MakeColorRandom()
{
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	Vector3s rand_color(100);
	for (int i = 0; i < 100; i++)
	{
		rand_color[i].x = rand() / 1.0 / RAND_MAX * 255;
		rand_color[i].y = rand() / 1.0 / RAND_MAX * 255;
		rand_color[i].z = rand() / 1.0 / RAND_MAX * 255;
	}
	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain() && fc->info().in_domain()
				&& fc->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
			t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
			t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
			t.m_Colors[0] = rand_color[fc->info().color_label % 100];
			t.m_Colors[1] = rand_color[fc->info().color_label % 100];
			t.m_Colors[2] = rand_color[fc->info().color_label % 100];
			m_Triangles.push_back(t);
		}
	}
}

void TriangulationCgal_Sideline::SetColor(ColorConstraint_sptrs& colors)
{
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
	Vector3s rand_color(100);
	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain() && fc->info().in_domain()
				&& fc->info().nesting_level != TRIANGLE_TRANSPARENT)
		{
			t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
			t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
			t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
			t.m_Colors[0] = colors[fc->info().color_label]->GetColorVector3();
			t.m_Colors[1] = colors[fc->info().color_label]->GetColorVector3();
			t.m_Colors[2] = colors[fc->info().color_label]->GetColorVector3();
			m_Triangles.push_back(t);
		}
	}
}

void TriangulationCgal_Sideline::RemoveShadingLine(vavImage& vimg)
{
	for (int i = 0; i < m_Lines.size(); ++i)
	{
		Line& now_line = m_Lines[i];
		int isBlack = 0;
		for (int j = 0; j < now_line.size(); j++)
		{
			if (vimg.IsBlackLine(now_line[j].x, now_line[j].y, 3))
			{
				isBlack++;
			}
			else if (vimg.IsBlackLine(now_line[j].x, now_line[j].y, 4))
			{
				isBlack++;
			}
			else if (vimg.IsBlackLine(now_line[j].x, now_line[j].y, 5))
			{
				isBlack++;
			}
		}
		if (isBlack < now_line.size() * 0.3)
		{
			m_Lines.erase(m_Lines.begin() + i);
			m_LinesWidth.erase(m_LinesWidth.begin() + i);
			i--;
		}
	}
}
