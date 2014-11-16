#include "PicMesh.h"
#include "TriangulationCgal_Sideline.h"
#include <auto_link_OpenMesh.hpp>

PicMesh::PicMesh(void)
{
    request_vertex_status();
    request_edge_status();
    request_face_status();
}


PicMesh::~PicMesh(void)
{
    release_vertex_status();
    release_edge_status();
    release_face_status();
}

void PicMesh::ReadFromSideline(TriangulationCgal_Sideline* ts)
{
    m_Lines = ts->m_Lines;
    m_i2s   = ts->m_i2s;
    m_ColorConstraint = ts->m_ColorConstraint;
    TriangulationCgal_Sideline::Finite_vertices_iterator vc;
    TriangulationCgal_Sideline::Finite_faces_iterator fc;
    vc = ts->m_Triangulation.finite_vertices_begin();
    int i = 0;
    for(; vc != ts->m_Triangulation.finite_vertices_end(); ++vc)
    {
        BasicMesh::Point p(vc->point()[0], vc->point()[1], 0);
        VertexHandle v = add_vertex(p);
        vc->info().idx = i;
        i++;
    }
    int maxregion = 0;
    fc = ts->m_Triangulation.finite_faces_begin();
    for(; fc != ts->m_Triangulation.finite_faces_end(); ++fc)
    {
        if(fc->is_in_domain())
        {
            BasicMesh::VHandle vh[3];
            vh[0] = BasicMesh::VHandle(fc->vertex(0)->info().idx);
            vh[1] = BasicMesh::VHandle(fc->vertex(1)->info().idx);
            vh[2] = BasicMesh::VHandle(fc->vertex(2)->info().idx);
            FaceHandle f = add_face(vh[0], vh[1], vh[2]);
            data(f).rid = fc->info().color_label;
            if(data(f).rid > maxregion)
            {
                maxregion = data(f).rid;
            }
        }
    }
    // 存下不同的 region
    m_Regions.resize(maxregion + 1);
    for(BasicMesh::FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        m_Regions[data(*fit).rid].push_back(*fit);
    }
    // 把兩邊的顏色填上
    m_LinesVH.resize(m_Lines.size());
    m_LinesHH.resize(m_Lines.size());
    for(int i = 0; i < m_Lines.size(); ++i)
    {
        printf("*");
        VHandles& tvhs = m_LinesVH[i];
        HHandles& thhs = m_LinesHH[i];
        Line& line = m_Lines[i];
        BasicMesh::Point now(line[0].x, line[0].y, 0);
        for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
        {
            if((point(*vit) - now).sqrnorm() < 0.01)
            {
                tvhs.push_back(*vit);
                break;
            }
        }
        int k = 0;
        for(int j = 1; j < line.size(); ++j)
        {
            k++;
            if(k > line.size() * 3)
            {
                break;
            }
            now = BasicMesh::Point(line[j].x, line[j].y, 0);
            BasicMesh::VHandle last = tvhs.back();
            double initdis = 1000000;
            double mindis = initdis;
            int minidx = -1;
            int ii = 0;
            for(VVIter vvit = vv_iter(last); vvit.is_valid(); ++vvit)
            {
                Point tp = point(*vvit);
                double dis = (point(*vvit) - now).sqrnorm();
                if(dis < mindis)
                {
                    mindis = dis;
                    minidx = ii;
                }
                ii++;
            }
            if(mindis > 1)
            {
                --j;
                //printf("-");
            }
//             if(mindis < initdis)
//             {
//                 printf("dis: %f\n", mindis);
//             }
            if(minidx >= 0)
            {
                ii = 0;
                for(VVIter vvit = vv_iter(last); vvit.is_valid(); ++vvit)
                {
                    if(ii == minidx)
                    {
                        thhs.push_back(find_halfedge(last, *vvit));
                        FHandle tfh = face_handle(thhs.back());
                        if(tfh.is_valid())
                        {
                            data(tfh).cid = m_i2s.right[i][0];
                        }
                        FHandle ofh = opposite_face_handle(thhs.back());
                        if(ofh.is_valid())
                        {
                            data(ofh).cid = m_i2s.left[i][0];
                        }
                        tvhs.push_back(*vvit);
                    }
                    ++ii;
                }
            }
        }
    }
	
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        ints rnum;
        ints count;
        for(int j = 0; j < m_Regions[i].size(); ++j)
        {
            int cid = data(m_Regions[i][j]).cid;
            if(cid != -1)
            {
                ints::iterator it = std::find(rnum.begin(), rnum.end(), cid);
                if(it != rnum.end())
                {
                    count[it - rnum.begin()]++;
                }
                else
                {
                    rnum.push_back(cid);
                    count.push_back(1);
                }
            }
        }
        if(!count.empty())
        {
            ints::iterator it = std::max_element(count.begin(), count.end());
            int maxid = it - count.begin();
            for(int j = 0; j < m_Regions[i].size(); ++j)
            {
                data(m_Regions[i][j]).cid = rnum[maxid];
            }
        }
    }
}

void PicMesh::MakeColor1()
{
    m_Trangles.clear();
    ColorTriangle t;
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int tid = data(*fit).cid;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(tid > 0)
            {
                t.color[c] = m_ColorConstraint[tid - 1].GetColorVector3(p[0], p[1]);
            }
            else
            {
                t.color[c] = Vector3(0, 0, 0);
            }
            ++c;
        }
        m_Trangles.push_back(t);
    }
}

void PicMesh::MakeColor2()
{
    m_Trangles.clear();
    ColorTriangle t;
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int tid = data(*fit).cid;
        int c = 0;
        if(tid > 0)
        {
            for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
            {
                Point p = point(*fvit);
                t.pts[c][0] = p[0];
                t.pts[c][1] = p[1];
                t.color[c] = m_ColorConstraint[tid - 1].GetColorVector3(p[0], p[1]);
                ++c;
            }
            m_Trangles.push_back(t);
        }
    }
}

void PicMesh::MappingMesh(PicMesh& pm, double x, double y)
{
    BasicMesh::VHandle lastV = *(--pm.vertices_end());
    for(BasicMesh::VIter pvit = vertices_begin(); pvit != vertices_end(); ++pvit)
    {
        BasicMesh::Point now = point(*pvit);
		now[0] += x;
		now[1] += y;
        double tdis = (pm.point(lastV) - now).sqrnorm();
        double mindis = tdis;
        for(; tdis > 1;)
        {
            int minidx = -1;
            for(VVIter vvit = pm.vv_iter(lastV); vvit.is_valid(); ++vvit)
            {
                Point tp = pm.point(*vvit);;
                double dis = (tp - now).sqrnorm();
                if(dis < mindis)
                {
                    mindis = dis;
                    minidx = (*vvit).idx();
                }
            }
            if(minidx >= 0)
            {
                tdis = mindis;
                lastV = BasicMesh::VHandle(minidx);
            }
            else
            {
                break;
            }
        }
        data(*pvit).mapid = lastV.idx();
    }
    BasicMesh::FHandle lastF = *(--pm.faces_end());
    for(BasicMesh::FIter pvit = faces_begin(); pvit != faces_end(); ++pvit)
    {
        BasicMesh::Point now = MidPoint(*pvit);
		now[0] += x;
		now[1] += y;
        double tdis = (pm.MidPoint(lastF) - now).sqrnorm();
        for(; tdis > 1;)
        {
            double mindis = tdis;
            int minidx = -1;
            int ii = 0;
            for(FFIter ffit = pm.ff_iter(lastF); ffit.is_valid(); ++ffit)
            {
                Point tp = pm.MidPoint(*ffit);
                double dis = (tp - now).sqrnorm();
                if(dis < mindis)
                {
                    mindis = dis;
                    minidx = (*ffit).idx();
                }
                ii++;
            }
            if(minidx >= 0)
            {
                tdis = mindis;
                lastF = BasicMesh::FHandle(minidx);
            }
            else
            {
                break;
            }
        }
        data(*pvit).mapid = lastF.idx();
    }
}

void PicMesh::GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2)
{
    printf("pm.n_vertices() %d\n", pm.n_vertices());
    out1.clear();
    out2.clear();
    ColorTriangle t;
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int tid = data(*fit).cid;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(tid > 0)
            {
                t.color[c] = m_ColorConstraint[tid - 1].GetColorVector3(p[0], p[1]);
            }
            else
            {
                t.color[c] = Vector3(0, 0, 0);
            }
            ++c;
        }
        out1.push_back(t);
    }
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int mapid = data(*fit).mapid;
        FHandle mfh(mapid);
        int tid = pm.data(mfh).cid;
		//int tid = data(*fit).cid;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            int mapid2 = data(*fvit).mapid;
            VHandle mvh(mapid2);
            Point p = pm.point(mvh);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(tid > 0)
            {
                t.color[c] = pm.m_ColorConstraint[tid - 1].GetColorVector3(p[0], p[1]);
            }
            else
            {
                t.color[c] = Vector3(0, 0, 0);
            }
            ++c;
        }
        out2.push_back(t);
    }
}

BasicMesh::Point PicMesh::MidPoint(FHandle fh)
{
    Point v[3];
    int k = 0;
    for(FVIter fv_it = fv_iter(fh); fv_it.is_valid() ; ++fv_it)
    {
        const Point& p = point(*fv_it);
        v[k++] = p;
    }
    return (v[0] + v[1] + v[2]) * 0.33333;
}

ColorTriangles PicMesh::Interpolation(ColorTriangles& c1, ColorTriangles& c2, double alpha)
{
    ColorTriangles res = c1;
    for(int i = 0; i < c1.size(); ++i)
    {
        ColorTriangle& t = res[i];
        t.color[0] = c1[i].color[0] * (1 - alpha) + c2[i].color[0] * alpha;
        t.color[1] = c1[i].color[1] * (1 - alpha) + c2[i].color[1] * alpha;
        t.color[2] = c1[i].color[2] * (1 - alpha) + c2[i].color[2] * alpha;
        t.pts[0] = c1[i].pts[0] * (1 - alpha) + c2[i].pts[0] * alpha;
        t.pts[1] = c1[i].pts[1] * (1 - alpha) + c2[i].pts[1] * alpha;
        t.pts[2] = c1[i].pts[2] * (1 - alpha) + c2[i].pts[2] * alpha;
    }
    return res;
}

