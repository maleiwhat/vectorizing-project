#include "PicMesh.h"
#include "TriangulationCgal_Sideline.h"
#include <auto_link_OpenMesh.hpp>
#include "vavImage.h"
#include "Line.h"

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
    // 把兩邊的顏色填上，標上end point
    m_LinesVH.resize(m_Lines.size());
    m_LinesHH.resize(m_Lines.size());
    for(int i = 0; i < m_Lines.size(); ++i)
    {
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
        ints idxs;
        Vector2s pts;
        for(int j = 1; j < line.size(); ++j)
        {
            idxs.push_back(j);
            k++;
            if(k > line.size() * 3)
            {
//                 for(int n = 0; n < idxs.size(); ++n)
//                 {
//                     printf("%d %.1f %.1f ", idxs[n], pts[n][0], pts[n][1]);
//                  if (n%5 == 0 && n > 0)
//                  {printf("\n");}
//                 }
                printf("f\n");
                break;
            }
            now = BasicMesh::Point(line[j].x, line[j].y, 0);
            BasicMesh::Point lastp = BasicMesh::Point(line[j - 1].x, line[j - 1].y, 0);
            BasicMesh::VHandle last = tvhs.back();
            double initdis = 10000;
            double mindis = initdis;
            int minidx = -1;
            int ii = 0;
            double pointdis = initdis;
            ii = 0;
            for(VVIter vvit = vv_iter(last); vvit.is_valid(); ++vvit, ii++)
            {
                Point tp = point(*vvit);
                Vector2 p(tp[0], tp[1]);
                double dis = (tp - now).norm() + (tp - lastp).norm();
                if(abs(dis - mindis) < 0.1)
                {
                    double npointdis = (tp - now).norm();
                    if(npointdis < pointdis)
                    {
                        pointdis = npointdis;
                        mindis = dis;
                        minidx = ii;
                    }
                }
                else if(dis < mindis)
                {
                    pointdis = (tp - now).norm();
                    mindis = dis;
                    minidx = ii;
                }
            }
            if(pointdis > 0.3)
            {
                --j;
            }
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
                        Point pp = point(*vvit);
                        pts.push_back(Vector2(pp[0], pp[1]));
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
    Vector3s colors;
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        colors.push_back(Vector3(rand() % 256, rand() % 256, rand() % 256));
    }
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int tid = data(*fit).cid;
        int rid = data(*fit).rid;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
//             if(tid > 0 && m_ColorConstraint.size() >= tid)
//             {
//                 t.color[c] = m_ColorConstraint[tid - 1].GetColorVector3(p[0], p[1]);
//             }
//             else
            if(rid >= 0 && rid < colors.size())
            {
                t.color[c] = colors[rid];
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

void PicMesh::MakeRegionLine(cv::Mat& img, double lmax)
{
    vavImage vimg(img);
    // mark boundary constraint
    for(VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        Point p = point(*vit);
        if(p[0] == 0 || p[1] == 0 || p[0] == m_w || p[1] == m_h)
        {
            data(*vit).constraint = 1;
        }
    }
    // mark constraint point
    for(int i = 0; i < m_LinesVH.size(); ++i)
    {
        for(int j = 0; j < m_LinesVH[i].size(); ++j)
        {
            data(m_LinesVH[i][j]).constraint = 1;
            data(m_LinesVH[i][j]).lineid = i;
            data(m_LinesVH[i][j]).lineidx = j;
        }
    }
    // mark end point
    for(int i = 0; i < m_LinesVH.size(); ++i)
    {
        data(m_LinesVH[i].front()).end = true;
        data(m_LinesVH[i].back()).end = true;
    }
    for(int i = 0; i < m_LinesVH.size(); ++i)
    {
        Line& nowline = m_Lines[i];
        int last = m_LinesVH[i].size() - 1;
        Vector2 out;
        bool res;
        if(data(m_LinesVH[i][0]).constraint > 0)
        {
            Point dir;
            if(m_LinesVH[i].size() > 3)
            {
                Vector2 v = nowline[1] - nowline[2];
                dir = Point(v[0], v[1], 0);
            }
            else
            {
                Vector2 v = nowline[0] - nowline[1];
                dir = Point(v[0], v[1], 0);
            }
            dir.normalize();
            res = ConnectOneRingConstraint1(vimg, m_LinesVH[i][0], m_LinesVH[i][1], out, dir, lmax);
            if(res)
            {
                nowline.insert(nowline.begin(), out);
            }
        }
        if(data(m_LinesVH[i][last]).constraint > 0)
        {
            int last2 = nowline.size() - 1;
            Point dir, orig = point(m_LinesVH[i][last]);
            Vector2 vOrig(orig[0], orig[1]);
            Vector2 checkOrig = nowline[last2];
            if(vOrig.squaredDistance(checkOrig) < 0.01)
            {
                if(m_LinesVH[i].size() > 3)
                {
                    Vector2 v = nowline[last2 - 1] - nowline[last2 - 2];
                    dir = Point(v[0], v[1], 0);
                }
                else
                {
                    Vector2 v = nowline[last2] - nowline[last2 - 1];
                    dir = Point(v[0], v[1], 0);
                }
                dir.normalize();
                res = ConnectOneRingConstraint1(vimg, m_LinesVH[i][last], m_LinesVH[i][last - 1], out, dir, lmax);
                if(res)
                {
                    nowline.insert(nowline.end(), out);
                }
            }
        }
    }
}

bool PicMesh::ConnectOneRingConstraint1(vavImage& vimg, VHandle vh, VHandle lastvh, Vector2& out, Point dir, double lmax)
{
    VHandles ends;
    VHandles constraints;
    VHandles alls;
    const double GradientThreshold = 1000;
    const double LINE_THRESHOLD = 5;
    bool isok = false;
    for(VVIter vvit = vv_iter(vh); vvit.is_valid(); ++vvit)
    {
        if(*vvit != lastvh)
        {
            if(data(*vvit).end)
            {
                Point px = point(*vvit) - point(vh);
                if(px.length() < lmax)
                {
                    ends.push_back(*vvit);
                    alls.push_back(*vvit);
                }
            }
            else if(data(*vvit).constraint > 0)
            {
                Point px = point(*vvit) - point(vh);
                if(px.length() < lmax)
                {
                    if(data(*vvit).lineid == data(vh).lineid)
                    {
                        if(abs(data(*vvit).lineidx - data(vh).lineidx) > LINE_THRESHOLD)
                        {
                            constraints.push_back(*vvit);
                            alls.push_back(*vvit);
                        }
                    }
                    else
                    {
                        constraints.push_back(*vvit);
                        alls.push_back(*vvit);
                    }
                }
            }
        }
    }
    // 移除同一條線的constraint點
    if(ends.size() > 0 && constraints.size() > 0)
    {
        for(int i = 0; i < ends.size(); ++i)
        {
            for(int j = 0; j < constraints.size(); ++j)
            {
                if(data(ends[i]).lineid == data(constraints[j]).lineid)
                {
                    constraints.erase(constraints.begin() + j);
                    j--;
                }
            }
        }
    }
    // 直走 距離小於3
    if(alls.size() > 0)
    {
        Point p = dir;
        p.normalize();
        Vector2 v(p[0], p[1]);
        doubles lens;
        for(int i = 0; i < alls.size(); ++i)
        {
            Point px = point(alls[i]) - point(vh);
            px.normalize();
            Vector2 v2(px[0], px[1]);
            lens.push_back(v.distance(v2));
        }
        double minlen = 999;
        int idx = 0;
        for(int i = 0; i < lens.size(); ++i)
        {
            if(minlen > lens[i])
            {
                minlen = lens[i];
                idx = i;
            }
        }
        Point op = point(alls[idx]);
        double dis = (op - point(vh)).length();
        if(dis < 3 && minlen < 0.3)
        {
            isok = true;
        }
        if(isok)
        {
            out = Vector2(op[0], op[1]);
            //data(vh).end = false;
            data(alls[idx]).end = true;
            data(alls[idx]).constraint += 1;
            return true;
        }
    }
    // 有邊有點 共邊點先連
    if(ends.size() > 0 && constraints.size() > 0)
    {
        for(int i = 0; i < ends.size(); ++i)
        {
            if(data(ends[i]).constraint > 1)
            {
                Point p = point(ends[i]);
                double dis = (p - point(vh)).length();
                bool isok2 = true;
                for(int j = 0; j < constraints.size(); ++j)
                {
                    if((point(constraints[j]) - point(vh)).length() * 1.5 < dis)
                    {
                        isok2 = false;
                        break;
                    }
                }
                if(isok2 && dis < LINE_THRESHOLD)
                {
                    isok = true;
                }
                if(isok)
                {
                    out = Vector2(p[0], p[1]);
                    data(vh).end = false;
                    data(ends[i]).constraint += 1;
                    return true;
                }
            }
        }
        for(int i = 0; i < ends.size(); ++i)
        {
            for(int j = 0; j < constraints.size(); ++j)
            {
                if(isConnection(ends[i], constraints[j]))
                {
                    Point p1 = dir;
                    p1.normalize();
                    Point p2 = point(ends[i]) - point(vh);
                    p2.normalize();
                    Point p3 = point(constraints[j]) - point(vh);
                    p3.normalize();
                    if((p1 - p3).length() < 0.7)
                    {
                        Point p = point(constraints[j]);
                        double dis = (p - point(vh)).length();
                        if(dis < LINE_THRESHOLD)
                        {
                            isok = true;
                        }
                        if(isok)
                        {
                            out = Vector2(p[0], p[1]);
                            data(vh).end = false;
                            data(constraints[j]).end = true;
                            data(constraints[j]).constraint += 1;
                            return true;
                        }
                    }
                }
            }
        }
    }
    // 點只有一個 就近就連
    if(ends.size() == 1)
    {
        Point p = point(ends[0]);
        if((p - point(vh)).length() < 6)
        {
            out = Vector2(p[0], p[1]);
            data(vh).end = false;
            data(ends[0]).end = false;
            return true;
        }
    }
    if(alls.size() > 0)
    {
        Point p = dir;
        p.normalize();
        Vector2 v(p[0], p[1]);
        doubles lens;
        for(int i = 0; i < alls.size(); ++i)
        {
            Point px = point(alls[i]) - point(vh);
            px.normalize();
            Vector2 v2(px[0], px[1]);
            lens.push_back(v.distance(v2));
        }
        double minlen = 999;
        int idx = 0;
        for(int i = 0; i < lens.size(); ++i)
        {
            if(minlen > lens[i])
            {
                minlen = lens[i];
                idx = i;
            }
        }
        Point op = point(alls[idx]);
        double dis = (op - point(vh)).length();
        if(dis > LINE_THRESHOLD)
        {
            if(minlen < 0.8)
            {
                isok = true;
            }
        }
        else
        {
            isok = true;
        }
        if(isok)
        {
            out = Vector2(op[0], op[1]);
            //data(vh).end = false;
            data(alls[idx]).end = true;
            data(alls[idx]).constraint += 1;
            return true;
        }
    }
    if(ends.size() > 1)
    {
        Point p = dir;
        p.normalize();
        Vector2 v(p[0], p[1]);
        doubles lens;
        for(int i = 0; i < ends.size(); ++i)
        {
            Point px = point(ends[i]) - point(vh);
            px.normalize();
            Vector2 v2(px[0], px[1]);
            lens.push_back(v.distance(v2));
        }
        double minlen = 99;
        int idx = 0;
        for(int i = 0; i < lens.size(); ++i)
        {
            if(minlen > lens[i])
            {
                minlen = lens[i];
                idx = i;
            }
        }
        Point op = point(ends[idx]);
        out = Vector2(op[0], op[1]);
        data(vh).end = false;
        data(ends[idx]).end = false;
        return true;
    }
    return false;
}



void PicMesh::SetSize(int w, int h)
{
    m_w = w;
    m_h = h;
}

bool PicMesh::isConnection(VHandle vh1, VHandle vh2)
{
    for(VVIter vvit = vv_iter(vh1); vvit.is_valid(); ++vvit)
    {
        if(*vvit == vh2)
        {
            return true;
        }
    }
    return false;
}

void PicMesh::MakeColor3()
{
    m_Trangles.clear();
    ColorTriangle t;
    Vector3s colors;
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        colors.push_back(Vector3(255, 255, 255));
    }
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = data(*fit).rid;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(rid >= 0 && m_ColorConstraint.size() > rid)
            {
                t.color[c] = m_ColorConstraint[rid].GetColorVector3(p[0], p[1]);
            }
            else if(rid >= 0 && rid < colors.size())
            {
                t.color[c] = colors[rid];
            }
            ++c;
        }
        m_Trangles.push_back(t);
    }
}

void PicMesh::BuildColorModels(cv::Mat& img)
{
    // mark boundary constraint
    for(int i = 0; i < m_LinesHH.size(); ++i)
    {
        HHandles& nowhh = m_LinesHH[i];
        for(int j = 0; j < nowhh.size(); ++j)
        {
            data(edge_handle(nowhh[j])).constraint = true;
        }
    }
    for(VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        Point p = point(*vit);
        if(p[0] == 0 || p[1] == 0 || p[0] == m_w || p[1] == m_h)
        {
            data(*vit).constraint = 1;
        }
    }
    // mark constraint point
    for(int i = 0; i < m_LinesVH.size(); ++i)
    {
        for(int j = 0; j < m_LinesVH[i].size(); ++j)
        {
            data(m_LinesVH[i][j]).constraint = 1;
            data(m_LinesVH[i][j]).lineid = i;
            data(m_LinesVH[i][j]).lineidx = j;
        }
    }
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        data(*vit).use = false;
    }
    vavImage vimg(img);
    m_ColorConstraint.clear();
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        m_ColorConstraint.push_back(ColorConstraint());
        ColorConstraint& cc = m_ColorConstraint.back();
        FHandles& region = m_Regions[i];
        for(int j = 0; j < region.size(); ++j)
        {
            for(FEIter fe_itr = fe_iter(region[j]) ; fe_itr.is_valid(); ++fe_itr)
            {
                BasicMesh::HalfedgeHandle _hedge = halfedge_handle(*fe_itr, 1);
                Vector3 cx;
                Point p1 = point(from_vertex_handle(_hedge));
                Point p2 = point(to_vertex_handle(_hedge));
                p1 = (p1 + p2) * 0.5;
                Vector2 pp(p1[0], p1[1]);
                bool res = GetLighterColor(img, from_vertex_handle(_hedge),
                                           to_vertex_handle(_hedge), cx);
                if(res)
                {
                    cc.AddPoint(pp[0], pp[1], cx);
                }
            }
            Point p = MidPoint(region[j]);
            Vector3 c = vimg.GetBilinearColor(p[0], p[1]);
            double t = c[0];
            c[0] = c[2];
            c[2] = t;
            cc.AddPoint(p[0], p[1], c);
            for(FVIter fvit = fv_iter(region[j]); fvit.is_valid(); ++fvit)
            {
                if(data(*fvit).constraint == 0 && !data(*fvit).use)
                {
                    data(*fvit).use = true;
                    Point p2 = point(*fvit);
                    Vector3 c = vimg.GetBilinearColor(p2[0] - 0.5, p2[1] - 0.5);
                    double t = c[0];
                    c[0] = c[2];
                    c[2] = t;
                    cc.AddPoint(p2[0], p2[1], c);
                }
            }
        }
    }
}

bool PicMesh::GetLighterColor(cv::Mat& img, VHandle vh1, VHandle vh2, Vector3& c)
{
    if(data(vh1).constraint > 0 && data(vh2).constraint > 0 && !data(edge_handle(find_halfedge(vh1, vh2))).constraint)
    {
        vavImage vimg(img);
        Point p1 = point(vh1);
        Point p2 = point(vh2);
        Vector2 v1(p1[0] + 1, p1[1]);
        Vector2 v2(p2[0], p2[1]);
        Vector3s colors;
        Vector2 step = (v2 - v1) / 10;
        double maxlight = 0;
        for(int i = 1; i < 9; ++i)
        {
            Vector2 dst = v1 + step * i;
            Vector3 tmp = vimg.GetBilinearColor(dst[0] - 0.5, dst[1] - 0.5);
            std::swap(tmp[0], tmp[2]);
            colors.push_back(tmp);
            double light = GetLight(tmp);
            if(maxlight < light)
            {
                maxlight = light;
                c = tmp;
            }
        }
//         std::sort(colors.begin(), colors.end());
//         c = colors[colors.size() * 0.8];
        return true;
    }
    return false;
}

void PicMesh::MakeColor4(cv::Mat& img)
{
    vavImage vimg(img);
    // mark boundary constraint
    for(VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        Point p = point(*vit);
        if(p[0] == 0 || p[1] == 0 || p[0] == m_w || p[1] == m_h)
        {
            data(*vit).constraint = 1;
        }
    }
    // mark constraint point
    for(int i = 0; i < m_LinesVH.size(); ++i)
    {
        for(int j = 0; j < m_LinesVH[i].size(); ++j)
        {
            data(m_LinesVH[i][j]).constraint = 1;
        }
    }
    m_Trangles.clear();
    ColorTriangle t;
    // get color far constraint
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            Point p = point(*vit);
            data(*vit).c2 =  vimg.GetBilinearColor(p[0] - 0.5, p[1] - 0.5);
        }
    }
    // blur c2
    lightblurC2();
	lightblurC2();
	lightblurC2();
    // get face color from vertex
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int count = 0;
        Vector3 avg;
        for(FVIter fv_itr = fv_iter(*fit) ; fv_itr.is_valid(); ++fv_itr)
        {
            if(data(*fv_itr).constraint == 0)
            {
                count++;
                avg += data(*fv_itr).c2;
            }
        }
        if(count > 0)
        {
            avg /= count;
            data(*fit).c2 = avg;
        }
    }
    // Dilation face color
    Dilation();
    Dilation();
    Dilation();
	Dilation();
    // get face color
    GetConstraintFaceColor(img);
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = data(*fit).rid;
        int c = 0;
        Vector3 tmp;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            t.color[c] = data(*fvit).c2;
            std::swap(t.color[c][0], t.color[c][2]);
            if(t.color[c] != Vector3())
            {
                tmp = t.color[c];
            }
            ++c;
        }
        if(tmp != Vector3())
        {
            for(int i = 0; i < 3; ++i)
            {
                if(t.color[i] == Vector3())
                {
                    t.color[i] = tmp;
                }
            }
        }
        else
        {
            tmp = data(*fit).c2;
            std::swap(tmp[0], tmp[2]);
            for(int i = 0; i < 3; ++i)
            {
                if(t.color[i] == Vector3())
                {
                    t.color[i] = tmp;
                }
            }
        }
        m_Trangles.push_back(t);
    }
}

double PicMesh::GetLineGradient(vavImage& vimg, VHandle vh1, VHandle vh2, double len)
{
    Point tp1 = point(vh1);
    Point tp2 = point(vh2);
    Point step = (tp2 - tp1) / 8;
    Line line;
    for(int i = 4; i < 7; ++i)
    {
        Point dst = tp1 + step * i;
        line.push_back(Vector2(dst[0], dst[1]));
    }
    Line normals = GetNormalsLen2(line);
    double res = 0;
    for(int i = 0; i < line.size(); ++i)
    {
        Vector2 p1 = line[i] + len * normals[i];
        Vector2 p2 = line[i] - len * normals[i];
        Vector3 c1 = vimg.GetBilinearColor(p1[0] - 0.5, p1[1] - 0.5);
        Vector3 c2 = vimg.GetBilinearColor(p2[0] - 0.5, p2[1] - 0.5);
        res += c1.squaredDistance(c2);
    }
    return res;
}

void PicMesh::blurC2()
{
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            int count = 1;
            Vector3 blur = data(*vit).c2;
            for(VVIter vv_itr = vv_iter(*vit) ; vv_itr.is_valid(); ++vv_itr)
            {
                if(data(*vv_itr).constraint == 0)
                {
                    count++;
                    blur += data(*vv_itr).c2;
                }
            }
            blur /= count;
            data(*vit).c3 = blur;
        }
    }
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            data(*vit).c2 = data(*vit).c3;
        }
    }
}

void PicMesh::lightblurC2()
{
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            int count = 1;
            Vector3 blur = data(*vit).c2;
            for(VVIter vv_itr = vv_iter(*vit) ; vv_itr.is_valid(); ++vv_itr)
            {
                if(data(*vv_itr).constraint == 0)
                {
                    count++;
                    blur += data(*vv_itr).c2;
                }
            }
            blur /= count;
            double light = GetLight(blur);
            count = 1;
            blur = data(*vit).c2;
            for(VVIter vv_itr = vv_iter(*vit) ; vv_itr.is_valid(); ++vv_itr)
            {
                if(data(*vv_itr).constraint == 0)
                {
                    if(GetLight(data(*vv_itr).c2) > light)
                    {
                        count += 2;
                        blur += data(*vv_itr).c2 * 2;
                    }
                    else
                    {
                        count++;
                        blur += data(*vv_itr).c2;
                    }
                }
            }
            blur /= count;
            data(*vit).c3 = blur;
        }
    }
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            data(*vit).c2 = data(*vit).c3;
        }
    }
}


void PicMesh::Dilation()
{
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).c2 == Vector3())
        {
            for(FFIter ff_itr = ff_iter(*fit) ; ff_itr.is_valid(); ++ff_itr)
            {
                if(data(*ff_itr).c2 != Vector3() && data(*ff_itr).rid == data(*fit).rid)
                {
                    data(*fit).c2 = data(*ff_itr).c2;
                    break;
                }
            }
        }
    }
}

void PicMesh::GetConstraintFaceColor(cv::Mat& img)
{
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        bool facenocolor = true;
        for(FVIter fv_itr = fv_iter(*fit) ; fv_itr.is_valid(); ++fv_itr)
        {
            if(data(*fv_itr).constraint == 0)
            {
                facenocolor = false;
                break;
            }
        }
        if(facenocolor && data(*fit).c2 == Vector3())
        {
            for(FEIter fe_itr = fe_iter(*fit) ; fe_itr.is_valid(); ++fe_itr)
            {
                BasicMesh::HalfedgeHandle _hedge = halfedge_handle(*fe_itr, 1);
                Vector3 cx;
                Point p1 = point(from_vertex_handle(_hedge));
                Point p2 = point(to_vertex_handle(_hedge));
                p1 = (p1 + p2) * 0.5;
                Vector2 pp(p1[0], p1[1]);
                bool res = GetLighterColor(img, from_vertex_handle(_hedge),
                                           to_vertex_handle(_hedge), cx);
                if(res)
                {
					std::swap(cx[0], cx[2]);
                    data(*fit).c2 = cx;
                    break;
                }
            }
        }
    }
}

