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
    // add 4 conner point
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
    m_LineSegs.clear();
    Insert_lines(m_Triangulation);
//     for(int i = 0; i < m_ImageSpline.m_PatchSplines.size(); ++i)
//     {
//         insert_polygon(m_Triangulation, m_ImageSpline, i);
//     }
//     for(int i = 0; i < m_ImageSpline.m_PatchSplinesInter.size(); ++i)
//     {
//         insert_polygonInter(m_Triangulation, m_ImageSpline, i);
//     }
    Mesher mesher(m_Triangulation);
    mesher.set_criteria(m_Criteria);
    ColorTriangle t;
    Finite_faces_iterator fc;
    mesher.refine_mesh();
    //mesher.mark_facets();
    mark_domains2(m_Triangulation);
    m_Triangles.clear();
    fc = m_Triangulation.finite_faces_begin();
    Vector3s rand_color(100);
    for(int i = 0; i < 100; i++)
    {
        rand_color[i].x = rand() / 1.0 / RAND_MAX * 255;
        rand_color[i].y = rand() / 1.0 / RAND_MAX * 255;
        rand_color[i].z = rand() / 1.0 / RAND_MAX * 255;
    }
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        if(fc->is_in_domain() && fc->info().in_domain() && fc->info().nesting_level != TRIANGLE_TRANSPARENT)
        {
            t.pts[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
            t.pts[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
            t.pts[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
//          if (fc->info().nesting_level >= 0 && m_ColorConstraint.size() > fc->info().nesting_level
//                          && m_ColorConstraint[fc->info().nesting_level].get())
            {
                t.color[0] = rand_color[fc->info().color_label % 100];
                t.color[1] = rand_color[fc->info().color_label % 100];
                t.color[2] = rand_color[fc->info().color_label % 100];
            }
//          else
//          {
//              t.m_Colors[0] = 255;
//              t.m_Colors[1] = 0;
//              t.m_Colors[2] = 0;
//          }
            m_Triangles.push_back(t);
        }
    }
    return 0;
}

void TriangulationCgal_Sideline::SetCriteria(float shapebound, float length)
{
    m_Criteria = Criteria(shapebound, length);
}

void TriangulationCgal_Sideline::Clear()
{
    m_SeedPoints.clear();
}

void TriangulationCgal_Sideline::Insert_lines(Triangulation& cdt)
{
    Point last;
    for(int i = 0; i < m_ImageSpline.m_LineFragments.size(); ++i)
    {
        Line& line = m_ImageSpline.m_LineFragments[i].m_Points;
        last = Point(line[0].x, line[0].y);
        Point start = last;
        Triangulation::Vertex_handle v_prev  = cdt.insert(last);
        for(int j = 0; j < line.size(); ++j)
        {
            Point now(line[j].x, line[j].y);
            if(now != last)
            {
                Triangulation::Vertex_handle vh = m_Triangulation.insert(now);
                m_Triangulation.insert_constraint(v_prev, vh);
                v_prev = vh;
                last = now;
            }
        }
    }
}

void TriangulationCgal_Sideline::insert_polygon(Triangulation& cdt, ImageSpline& m_ImageSpline, int idx)
{
    PatchSpline& ps = m_ImageSpline.m_PatchSplines[idx];
    LineIndex start_idx = ps.m_LineIndexs.front();
    Point last;
    if(start_idx.m_Forward)
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
    if(v_prev->info().nesting_level == -1)
    {
        v_prev->info().nesting_level = idx;
    }
    for(auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
    {
        Line pts = m_ImageSpline.m_LineFragments[it->m_id].m_Points;
        // show normal
//      for (int j = 1; j < pts.size(); ++j)
//      {
//          Vector2 right = Quaternion::GetRotation(pts[j] - pts[j - 1], 90);
//          m_LineSegs.push_back(LineSeg(pts[j], pts[j] + right*3));
//      }
        if(it->m_Forward)
        {
            for(auto it2 = pts.begin(); it2 != pts.end(); ++it2)
            {
                Point now(it2->x, it2->y);
                if(now != last)
                {
                    Triangulation::Vertex_handle vh = m_Triangulation.insert(now);
                    //assert(vh->info().nesting_level == -1 || vh->info().nesting_level == idx);
                    if(vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
                    {
                        vh->info().nesting_level = idx;
                        if(abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
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
            for(auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
            {
                Point now(it2->x, it2->y);
                if(now != last)
                {
                    Triangulation::Vertex_handle vh = m_Triangulation.insert(now);
                    //assert(vh->info().nesting_level == -1 || vh->info().nesting_level == idx);
                    if(vh->info().nesting_level == -1 || vh->info().nesting_level == idx)
                    {
                        vh->info().nesting_level = idx;
                        if(abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
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
    if(start != last)
    {
        Triangulation::Vertex_handle vh = m_Triangulation.insert(start);
        m_Triangulation.insert_constraint(v_prev, vh);
        last = start;
    }
    assert(start == last);
}

void TriangulationCgal_Sideline::insert_polygonInter2(Triangulation& cdt, ImageSpline& is, PatchSpline& ps)
{
    const int NESTING_LEVEL = TRIANGLE_TRANSPARENT;
    if(ps.m_LineIndexs.empty())
    {
        return;
    }
    LineIndex start_idx = ps.m_LineIndexs.front();
    Point last;
    if(start_idx.m_Forward)
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
    for(auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
    {
        Line pts = is.m_LineFragments[it->m_id].m_Points;
        if(it->m_Forward)
        {
            for(auto it2 = pts.begin(); it2 != pts.end(); ++it2)
            {
                Point now(it2->x, it2->y);
                if(now != last)
                {
                    Triangulation::Vertex_handle vh = m_Triangulation.insert(now);
                    if(vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
                    {
                        vh->info().nesting_level = NESTING_LEVEL;
                        if(abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
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
            for(auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
            {
                Point now(it2->x, it2->y);
                if(now != last)
                {
                    Triangulation::Vertex_handle vh = m_Triangulation.insert(now);
                    if(vh->info().nesting_level == -1 || vh->info().nesting_level == NESTING_LEVEL)
                    {
                        vh->info().nesting_level = NESTING_LEVEL;
                        if(abs(last.hx() - now.hx()) < 5 && abs(last.hy() - now.hy()) < 5)
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
    assert(start == last);
}

void TriangulationCgal_Sideline::insert_polygonInter(Triangulation& cdt, ImageSpline& is, int idx)
{
    PatchSplines& pss = is.m_PatchSplinesInter[idx];
    for(int i = 0; i < pss.size(); ++i)
    {
        insert_polygonInter2(cdt, is, pss[i]);
    }
}

void TriangulationCgal_Sideline::insert_polygon(Triangulation& cdt, const Polygon& polygon, int domain)
{
    if(polygon.is_empty())
    {
        return;
    }
    Triangulation::Vertex_handle v_prev = cdt.insert(*CGAL::cpp0x::prev(polygon.vertices_end()));
    for(Polygon::Vertex_iterator vit = polygon.vertices_begin();
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
void    TriangulationCgal_Sideline::mark_domains(Triangulation& ct, Triangulation::Face_handle start, int index,
        std::list<Triangulation::Edge>& border)
{
    if(start->info().nesting_level != TRIANGLE_NOT_INIT)
    {
        return;
    }
    std::list<Triangulation::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty())
    {
        Triangulation::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            fh->info().nesting_level = index;
            for(int i = 0; i < 3; i++)
            {
                Triangulation::Edge e(fh, i);
                Triangulation::Face_handle n = fh->neighbor(i);
                if(n->info().nesting_level == TRIANGLE_NOT_INIT)
                {
                    if(ct.is_constrained(e))
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
    for(Triangulation::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it)
    {
        it->info().nesting_level = TRIANGLE_NOT_INIT;
    }
    int index = 0;
    std::list<Triangulation::Edge> border;
    Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        int domain = fc->vertex(0)->info().nesting_level;
        domain = fc->vertex(1)->info().nesting_level == 0 ? fc->vertex(1)->info().nesting_level : domain;
        domain = fc->vertex(2)->info().nesting_level == 0 ? fc->vertex(2)->info().nesting_level : domain;
        if(TRIANGLE_NOT_INIT == fc->info().nesting_level && domain == 0)
        {
            mark_domains(cdt, fc, domain, border);
            break;
        }
    }
    while(! border.empty())
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
        if(transparent >= 2)
        {
            mark_domains(cdt, n, TRIANGLE_TRANSPARENT, border);
        }
        else if(n->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            //mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
            printf("domain: %d\n", domain);
            if(domain != -1)
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

void TriangulationCgal_Sideline::AddColorConstraint(ColorConstraint_sptr constraint)
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


void    TriangulationCgal_Sideline::mark_domains2(Triangulation& ct, Triangulation::Face_handle start, int index,
        std::list<Triangulation::Edge>& border, int color_label)
{
    if(start->info().nesting_level != TRIANGLE_NOT_INIT)
    {
        return;
    }
    std::list<Triangulation::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty())
    {
        Triangulation::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            fh->info().nesting_level = index;
            fh->info().color_label = color_label;
            for(int i = 0; i < 3; i++)
            {
                Triangulation::Edge e(fh, i);
                Triangulation::Face_handle n = fh->neighbor(i);
                if(n->info().nesting_level == TRIANGLE_NOT_INIT)
                {
                    if(ct.is_constrained(e))
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

int TriangulationCgal_Sideline::mark_domains2(Triangulation& cdt)
{
    for(Triangulation::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it)
    {
        it->info().nesting_level = TRIANGLE_NOT_INIT;
    }
    int cc = 0;
    int index = 1;
    std::list<Triangulation::Edge> border;
    Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        if(TRIANGLE_NOT_INIT == fc->info().nesting_level)
        {
            mark_domains2(cdt, fc, index++, border, cc++);
            break;
        }
    }
    while(! border.empty())
    {
        Triangulation::Edge e = border.front();
        border.pop_front();
        Triangulation::Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            mark_domains2(cdt, n, e.first->info().nesting_level + 1, border, cc++);
        }
    }
    m_RegionCount = cc;
	printf("m_RegionCount %d\n",m_RegionCount);
    return cc;
}


void TriangulationCgal_Sideline::MakeColorSequential()
{
    m_Triangles.clear();
    ColorTriangle t;
    Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        if(fc->is_in_domain() && fc->info().in_domain()
                && fc->info().nesting_level != TRIANGLE_TRANSPARENT)
        {
            t.pts[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
            t.pts[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
            t.pts[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
            int label = fc->info().color_label;
            Vector3 vm;
            vm.x = label % 255;
            vm.y = (label / 255) % 255;
            vm.z = label / 255 / 255;
            t.color[0] = vm;
            t.color[1] = vm;
            t.color[2] = vm;
            m_Triangles.push_back(t);
        }
    }
}

Triangles2d TriangulationCgal_Sideline::GetAllRegion()
{
    Triangles2d res(m_RegionCount);
    Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        int idx = fc->info().color_label;
        if(idx < res.size() && idx >= 0)
        {
            Triangle t;
            t.pts[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
            t.pts[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
            t.pts[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
            res[idx].push_back(t);
        }
    }
	return res;
}
