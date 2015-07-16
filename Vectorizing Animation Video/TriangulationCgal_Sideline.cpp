#include "TriangulationCgal_Sideline.h"
#include "algSpline2d.h"
#include "math/Quaternion.h"
#include "curve/HSplineCurve.h"
#include "Line.h"

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
    CTriangulation::Vertex_handle v1 = m_Triangulation.insert(lu);
    CTriangulation::Vertex_handle v2 = m_Triangulation.insert(ld);
    CTriangulation::Vertex_handle v3 = m_Triangulation.insert(ru);
    CTriangulation::Vertex_handle v4 = m_Triangulation.insert(rd);
    v1->info().nesting_level = 0;
    v2->info().nesting_level = 0;
    v3->info().nesting_level = 0;
    v4->info().nesting_level = 0;
//     m_Triangulation.insert_constraint(v1, v2);
//     m_Triangulation.insert_constraint(v2, v3);
//     m_Triangulation.insert_constraint(v3, v4);
//     m_Triangulation.insert_constraint(v4, v1);
    Insert_lines(m_Triangulation);
    Mesher mesher(m_Triangulation);
    mesher.set_criteria(m_Criteria);
    //mesher.mark_facets();
    mesher.refine_mesh();
    mark_domains2(m_Triangulation);
    for(int i = 0; i < m_Lines.size(); ++i)
    {
        Line& line = m_Lines[i];
        for(int j = 0; j < line.size(); ++j)
        {
            Point now(line[j].x, line[j].y);
            CTriangulation::Vertex_handle vh = m_Triangulation.nearest_vertex(now);
        }
    }

    return 0;
}

int TriangulationCgal_Sideline::Compute2()
{
    m_Triangulation.clear();
    Vertex_handles vhs;
    // add 4 conner point
    for(int y = 5; y < m_h - 1; y += 5)
    {
		int sx = 5;
		if (y % 2 == 0)
		{
			sx = 3;
		}
        for(int x = sx; x < m_w - 1; x += 5)
        {
            Point pp1(x, y);
            m_Triangulation.insert(pp1);
        }
    }
    Insert_lines(m_Triangulation);
    Mesher mesher(m_Triangulation);
    mesher.set_criteria(m_Criteria);
    mesher.mark_facets();
    //mesher.refine_mesh();
    mark_domains2(m_Triangulation);
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

void TriangulationCgal_Sideline::Insert_lines(CTriangulation& cdt)
{
    Point last;
    for(int i = 0; i < m_Lines.size(); ++i)
    {
        Line& line = m_Lines[i];
        last = Point(line[0].x, line[0].y);
        Point start = last;
        CTriangulation::Vertex_handle v_prev  = cdt.insert(last);
        for(int j = 0; j < line.size(); ++j)
        {
            Point now(line[j].x, line[j].y);
            if(now != last)
            {
                CTriangulation::Vertex_handle vh = m_Triangulation.insert(now);
                vh->info().nesting_level = i;
                m_Triangulation.insert_constraint(v_prev, vh);
                v_prev = vh;
                last = now;
            }
        }
    }
}

void TriangulationCgal_Sideline::insert_polygon(CTriangulation& cdt, const Polygon& polygon, int domain)
{
    if(polygon.is_empty())
    {
        return;
    }
    CTriangulation::Vertex_handle v_prev = cdt.insert(*CGAL::cpp0x::prev(polygon.vertices_end()));
    for(Polygon::Vertex_iterator vit = polygon.vertices_begin();
            vit != polygon.vertices_end(); ++vit)
    {
        CTriangulation::Vertex_handle vh = cdt.insert(*vit);
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
void    TriangulationCgal_Sideline::mark_domains(CTriangulation& ct, CTriangulation::Face_handle start, int index,
        std::list<CTriangulation::Edge>& border)
{
    std::list<CTriangulation::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty())
    {
        CTriangulation::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            fh->info().nesting_level = index;
            for(int i = 0; i < 3; i++)
            {
                CTriangulation::Edge e(fh, i);
                CTriangulation::Face_handle n = fh->neighbor(i);
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

void    TriangulationCgal_Sideline::mark_domains(CTriangulation& cdt)
{
    for(CTriangulation::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it)
    {
        it->info().nesting_level = TRIANGLE_NOT_INIT;
    }
    int index = 0;
    std::list<CTriangulation::Edge> border;
    Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        if(TRIANGLE_NOT_INIT != fc->info().nesting_level)
        {
            mark_domains(cdt, fc, fc->info().nesting_level, border);
        }
    }
//     while(! border.empty())
//     {
//         Triangulation::Edge e = border.front();
//         border.pop_front();
//         Triangulation::Face_handle n = e.first->neighbor(e.second);
//         int domain = e.first->vertex(0)->info().nesting_level;
//         domain = e.first->vertex(1)->info().nesting_level > domain ? e.first->vertex(1)->info().nesting_level : domain;
//         domain = e.first->vertex(2)->info().nesting_level > domain ? e.first->vertex(2)->info().nesting_level : domain;
//         int transparent = e.first->vertex(0)->info().nesting_level == TRIANGLE_TRANSPARENT;
//         transparent += e.first->vertex(1)->info().nesting_level == TRIANGLE_TRANSPARENT;
//         transparent += e.first->vertex(2)->info().nesting_level == TRIANGLE_TRANSPARENT;
//         if(transparent >= 2)
//         {
//             mark_domains(cdt, n, TRIANGLE_TRANSPARENT, border);
//         }
//         else if(n->info().nesting_level == TRIANGLE_NOT_INIT)
//         {
//             //mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
//             printf("domain: %d\n", domain);
//             if(domain != -1)
//             {
//                 mark_domains(cdt, n, domain, border);
//             }
//         }
//     }
}

void TriangulationCgal_Sideline::AddPatch(Patch& cvps)
{
    m_Patch.push_back(cvps);
}

void TriangulationCgal_Sideline::SetSize(int w, int h)
{
    m_w = w;
    m_h = h;
}


void    TriangulationCgal_Sideline::mark_domains2(CTriangulation& ct, CTriangulation::Face_handle start, int index,
        std::list<CTriangulation::Edge>& border, int color_label)
{
    if(start->info().nesting_level != TRIANGLE_NOT_INIT)
    {
        return;
    }
    std::list<CTriangulation::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty())
    {
        CTriangulation::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            fh->info().nesting_level = index;
            fh->info().color_label = color_label;
            for(int i = 0; i < 3; i++)
            {
                CTriangulation::Edge e(fh, i);
                CTriangulation::Face_handle n = fh->neighbor(i);
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

int TriangulationCgal_Sideline::mark_domains2(CTriangulation& cdt)
{
    for(CTriangulation::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it)
    {
        it->info().nesting_level = TRIANGLE_NOT_INIT;
    }
    int cc = 0;
    int index = 1;
    std::list<CTriangulation::Edge> border;
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
        CTriangulation::Edge e = border.front();
        border.pop_front();
        CTriangulation::Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == TRIANGLE_NOT_INIT)
        {
            mark_domains2(cdt, n, e.first->info().nesting_level + 1, border, cc++);
        }
    }
    m_RegionCount = cc;
    printf("m_RegionCount %d ", m_RegionCount);
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

void TriangulationCgal_Sideline::AddLines(Lines& is)
{
    m_Lines.insert(m_Lines.end(), is.begin(), is.end());
}

void TriangulationCgal_Sideline::SetColor(ColorConstraints& colors)
{
    m_ColorConstraint = colors;
}

void TriangulationCgal_Sideline::MakeColorByModel()
{
    ColorTriangle t;
    m_Triangles.clear();
    Finite_faces_iterator fc;
    fc = m_Triangulation.finite_faces_begin();
    for(; fc != m_Triangulation.finite_faces_end(); ++fc)
    {
        if(fc->is_in_domain())
        {
            t.pts[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
            t.pts[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
            t.pts[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
            int idx = fc->info().color_label;
            if(idx >= 0 && idx < m_ColorConstraint.size())
            {
                t.color[0] = m_ColorConstraint[idx].GetColorVector3(t.pts[0].x, t.pts[0].y);
                t.color[1] = m_ColorConstraint[idx].GetColorVector3(t.pts[1].x, t.pts[1].y);
                t.color[2] = m_ColorConstraint[idx].GetColorVector3(t.pts[2].x, t.pts[2].y);
            }
            m_Triangles.push_back(t);
        }
    }
}
