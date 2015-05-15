#include "PicMesh.h"
#include "TriangulationCgal_Sideline.h"
#include <auto_link_OpenMesh.hpp>
#include "vavImage.h"
#include "Line.h"
#include <algorithm>

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
                        data(edge_handle(thhs.back())).constraint = 1;
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
        data(*fit).c2 = colors[rid];
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
    BasicMesh::VHandle lastV(pm.n_vertices() / 2);
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
    for(BasicMesh::FIter pfit = faces_begin(); pfit != faces_end(); ++pfit)
    {
        BasicMesh::Point now = MidPoint(*pfit);
        now[0] += x;
        now[1] += y;
        BasicMesh::VHandle lastV(pm.n_vertices() / 2);
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
        int minidx = -1;
        int ridx = -1;
        mindis = 999999;
        for(VFIter ffit = pm.vf_iter(lastV); ffit.is_valid(); ++ffit)
        {
            Point tp = pm.MidPoint(*ffit);
            double dis = (tp - now).sqrnorm();
            if(dis < mindis)
            {
                mindis = dis;
                minidx = (*ffit).idx();
                ridx = pm.data(*ffit).rid;
            }
        }
        data(*pfit).mapid = ridx;
        BasicMesh::FHandle lastF(minidx);
        if(minidx >= 0)
        {
            //data(*pfit).c2 = pm.data(lastF).c2;
            data(*pfit).rid2 = pm.data(lastF).rid;
        }
    }
    m_MapingRegionIDs.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        FHandles& region = m_Regions[i];
        if(region.size() == 0)
        {
            continue;
        }
        Vector3 sum;
        int step = region.size() / 20;
        if(step < 1)
        {
            step = 1;
        }
        Vector3s colors;
        ints idxs;
        ints cuts;
        int count = 0;
        for(int j = 0; j < region.size(); j += step)
        {
            bool hasrepeat = false;
            int q = 0;
            for(; q < idxs.size(); ++q)
            {
                if(data(region[j]).rid2 == idxs[q])
                {
                    hasrepeat = true;
                    break;;
                }
            }
            if(!hasrepeat)
            {
                idxs.push_back(data(region[j]).rid2);
                colors.push_back(data(region[j]).c2);
                cuts.push_back(1);
            }
            else
            {
                cuts[q]++;
            }
            sum += data(region[j]).c2;
            count++;
        }
        sum /= count;
        Vector3 dst;
        double dis = 9999999;
        for(int j = 0; j < region.size(); ++j)
        {
            double ndis = sum.squaredDistance(data(region[j]).c2);
            if(ndis < dis)
            {
                dst = data(region[j]).c2;
                dis = ndis;
            }
        }
        int maxe = std::max_element(cuts.begin(), cuts.end()) - cuts.begin();
        m_MapingRegionIDs[i] = idxs[maxe];
        for(int j = 0; j < region.size(); ++j)
        {
            //data(region[j]).c2 = colors[maxe];
        }
    }
    return;
    // 畫出合拼的region
    for(BasicMesh::FIter pfit = faces_begin(); pfit != faces_end(); ++pfit)
    {
        if(data(*pfit).c2 == Vector3())
        {
            BasicMesh::Point now = MidPoint(*pfit);
            now[0] += x;
            now[1] += y;
            BasicMesh::VHandle lastV(pm.n_vertices() / 2);
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
            int minidx = -1;
            int ridx = -1;
            mindis = 999999;
            for(VFIter ffit = pm.vf_iter(lastV); ffit.is_valid(); ++ffit)
            {
                Point tp = pm.MidPoint(*ffit);
                double dis = (tp - now).sqrnorm();
                if(dis < mindis)
                {
                    mindis = dis;
                    minidx = (*ffit).idx();
                    ridx = pm.data(*ffit).rid;
                }
            }
            BasicMesh::FHandle lastF(minidx);
            //data(*pfit).c2 = pm.data(lastF).c2;
            data(*pfit).c2 = Vector3(0, 0, 255);
        }
    }
}

void PicMesh::MappingMeshByColor(PicMesh& pm, double x, double y)
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
        if(data(*pvit).c2 != Vector3() && pm.data(lastV).c2 != Vector3())
        {
            if(data(*pvit).c2.squaredDistance(pm.data(lastV).c2) < 1000)
            {
                data(*pvit).mapid = lastV.idx();
            }
        }
        else if(data(*pvit).c2 == Vector3() && pm.data(lastV).c2 != Vector3())
        {
        }
        else
        {
            data(*pvit).mapid = lastV.idx();
        }
    }
    BasicMesh::FHandle lastF = *(--pm.faces_end());
    for(BasicMesh::FIter pfit = faces_begin(); pfit != faces_end(); ++pfit)
    {
        BasicMesh::Point now = MidPoint(*pfit);
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
        if(data(*pfit).c2 != Vector3() && pm.data(lastF).c2 != Vector3())
        {
            if(data(*pfit).c2.squaredDistance(pm.data(lastF).c2) < 1000)
            {
                data(*pfit).mapid = lastF.idx();
            }
        }
        else
        {
            data(*pfit).mapid = lastF.idx();
        }
    }
}

void PicMesh::GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2, double x, double y)
{
    //printf("pm.n_vertices() %d\n", pm.n_vertices());
    out1.clear();
    out2.clear();
    ColorTriangle t;
    out1 = m_Trangles;
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        Vector3 tmp;
        int mapid = data(*fit).mapid;
        FHandle mfh(mapid);
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            int mapid2 = data(*fvit).mapid;
            if(mapid2 >= 0)
            {
                VHandle mvh(mapid2);
                Point p = pm.point(mvh);
                t.pts[c][0] = p[0];
                t.pts[c][1] = p[1];
                t.color[c] = pm.data(mvh).c2;
            }
            else
            {
                Point p = point(*fvit);
                t.pts[c][0] = p[0] + x;
                t.pts[c][1] = p[1] + y;
                t.color[c] = out1[out2.size()].color[c];
            }
            if(t.color[c] != Vector3())
            {
                tmp = t.color[c];
            }
            ++c;
        }
        for(int i = 0; i < 3; ++i)
        {
            if(t.color[i] == Vector3())
            {
                t.color[i] = tmp;
                t.color[i] = out1[out2.size()].color[i];
            }
        }
//         if(tmp != Vector3())
//         {
//             for(int i = 0; i < 3; ++i)
//             {
//                 if(t.color[i] == Vector3())
//                 {
//                     t.color[i] = tmp;
//                 }
//             }
//         }
//         else if(mapid >= 0 && mapid < (*pm.faces_end()).idx())
//         {
//             tmp = pm.data(mfh).c2;
//             for(int i = 0; i < 3; ++i)
//             {
//                 if(t.color[i] == Vector3())
//                 {
//                     t.color[i] = tmp;
//                 }
//             }
//         }
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
    Point res = (v[0] + v[1] + v[2]) * 0.33333;
    return res;
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
        cc.BuildModel();
    }
}


void PicMesh::BuildColorModels()
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
    m_ColorConstraint.clear();
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        m_ColorConstraint.push_back(ColorConstraint());
        ColorConstraint& cc = m_ColorConstraint.back();
        FHandles& region = m_Regions[i];
        for(int j = 0; j < region.size(); ++j)
        {
            if(data(region[j]).c2 != Vector3())
            {
                Point p = MidPoint(region[j]);
                cc.AddPoint(p[0], p[1], data(region[j]).c2);
            }
            for(FVIter fvit = fv_iter(region[j]); fvit.is_valid(); ++fvit)
            {
                if(data(*fvit).c2 != Vector3())
                {
                    Point p2 = point(*fvit);
                    cc.AddPoint(p2[0], p2[1], data(*fvit).c2);
                }
            }
        }
        cc.BuildModel();
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
            Vector3 c2 = vimg.GetBilinearColor(p[0] - 0.5, p[1] - 0.5);
            std::swap(c2[0], c2[2]);
            if(c2 == Vector3())
            {
                c2.x = 1;
            }
            data(*vit).c2 = c2;
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
    blurFaceC2();
    blurFaceC2();
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

void PicMesh::MakeColor5(cv::Mat& img)
{
    vavImage vimg(img);
    m_Trangles.clear();
    for(int i = 0; i < m_DifferentRegionIDs.size(); ++i)
    {
        FHandles region = m_Regions[m_DifferentRegionIDs[i]];
        for(FHandles::iterator fit = region.begin(); fit != region.end(); ++fit)
        {
            ColorTriangle t;
            int rid = data(*fit).rid;
            int c = 0;
            Vector3 tmp;
            for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
            {
                Point p = point(*fvit);
                t.pts[c][0] = p[0];
                t.pts[c][1] = p[1];
                t.color[c] = data(*fvit).c2;
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
                        count += 5;
                        blur += data(*vv_itr).c2 * 5;
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
        if(data(*fit).mark == 0)
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
                    if(cx == Vector3())
                    {
                        cx.x = 1;
                    }
                    data(*fit).c2 = cx;
                    data(*fit).mark = -2;
                    break;
                }
            }
        }
    }
}

void PicMesh::blurFaceC2x()
{
    for(BasicMesh::FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).mark == -2)
        {
            int count = 1;
            Vector3 blur = data(*fit).c2;
            for(FFIter ff_itr = ff_iter(*fit) ; ff_itr.is_valid(); ++ff_itr)
            {
                if(data(*ff_itr).c2 != Vector3())
                {
                    count++;
                    blur += data(*ff_itr).c2;
                }
            }
            blur /= count;
            data(*fit).c3 = blur;
        }
    }
    for(BasicMesh::FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).mark == -2)
        {
            data(*fit).c2 = data(*fit).c3;
        }
    }
}

void PicMesh::blurFaceC2()
{
    for(BasicMesh::FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).c2 != Vector3())
        {
            int count = 1;
            Vector3 blur = data(*fit).c2;
            for(FFIter ff_itr = ff_iter(*fit) ; ff_itr.is_valid(); ++ff_itr)
            {
                if(data(*ff_itr).c2 != Vector3())
                {
                    count++;
                    blur += data(*ff_itr).c2;
                }
            }
            blur /= count;
            data(*fit).c3 = blur;
        }
    }
    for(BasicMesh::FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).c2 != Vector3())
        {
            data(*fit).c2 = data(*fit).c3;
        }
    }
}

void PicMesh::MarkDifferentRegion1(vavImage& vimg, double v, double x, double y)
{
    m_DifferentRegionIDs.clear();
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        double sum = 0;
        FHandles& region = m_Regions[i];
        int count = 0;
        for(int j = 0; j < region.size(); ++j)
        {
            for(FVIter fvit = fv_iter(region[j]); fvit.is_valid(); ++fvit)
            {
                Vector3 c1 = data(*fvit).c2;
                if(c1 != Vector3())
                {
                    Point p = point(*fvit);
                    Vector3 c2 = vimg.GetBilinearColor(p[0] + x, p[1] + y);
                    std::swap(c2[0], c2[2]);
                    sum += c1.distance(c2);
                    count++;
                }
            }
        }
        sum /= count;
        if(sum > v)
        {
            m_DifferentRegionIDs.push_back(i);
        }
    }
    printf("m_Regions %d m_DifferentRegionIDs %d\n",
           m_Regions.size(), m_DifferentRegionIDs.size());
}

void PicMesh::MarkDifferentRegion2(PicMesh& pm, double v, double x, double y)
{
    BasicMesh::VHandle lastV = *(--pm.vertices_end());
    m_DifferentRegionIDs.clear();
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        double sum = 0;
        FHandles& region = m_Regions[i];
        int count = 0;
        for(int j = 0; j < region.size(); ++j)
        {
            for(FVIter fvit = fv_iter(region[j]); fvit.is_valid(); ++fvit)
            {
                Vector3 c1 = data(*fvit).c2;
                if(c1 != Vector3())
                {
                    BasicMesh::Point now = point(*fvit);
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
                    Vector3 c2 = pm.data(lastV).c2;
                    sum += c1.distance(c2);
                    count++;
                }
            }
        }
        sum /= count;
        if(sum > v)
        {
            m_DifferentRegionIDs.push_back(i);
        }
    }
    printf("m_Regions %d m_DifferentRegionIDs %d\n",
           m_Regions.size(), m_DifferentRegionIDs.size());
}

void PicMesh::MakeColor6(cv::Mat& img)
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
            Vector3 c2 = vimg.GetBilinearColor(p[0] - 0.5, p[1] - 0.5);
            std::swap(c2[0], c2[2]);
            if(c2 == Vector3())
            {
                c2.x = 1;
            }
            data(*vit).c2 = c2;
        }
    }
    // blur c2
    lightblurC2();
    lightblurC2();
    lightblurC2();

//  removeblackC2();
//  removeblackC2();
//  removeblackC2();
    BuildColorModels();
    MarkColoredFace();
    FillConstraintRegion();
//
//     for(int i = 0; i < m_Regions.size(); ++i)
//     {
//         double sum = 0;
//         FHandles& region = m_Regions[i];
//         if(region.empty())
//         {
//             continue;
//         }
//         int count = 0;
//         if(data(region[0]).mark == 0)
//         {
//             Vector3 sumc;
//             for(int j = 0; j < region.size(); ++j)
//             {
//                 Point p = MidPoint(region[j]);
//                 Vector3 c = vimg.GetBilinearColor(p[0] - 0.5, p[1] - 0.5);
//                 std::swap(c[0], c[2]);
//              sumc += c;
//             }
//          sumc /= region.size();
//             for(int j = 0; j < region.size(); ++j)
//             {
//                 data(region[j]).mark = -2;
//              data(region[j]).c2 = sumc;
//             }
//         }
//     }
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = data(*fit).rid;
        int c = 0;
        int c0 = 0;
        BasicMesh::Point mid = MidPoint(*fit);
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];

            if(data(*fit).mark > 0 && rid >= 0 && m_ColorConstraint.size() > rid)
            {
                t.color[c] = m_ColorConstraint[rid].GetColorVector3(p[0], p[1], mid[0], mid[1]);
            }
            else if(data(*fit).mark < 0)
            {
                t.color[c] = data(*fit).c2;
            }
            ++c;

        }
        if(data(*fit).mark != 0)
        {
            m_Trangles.push_back(t);
        }
    }
}

void PicMesh::removeblackC2()
{
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0)
        {
            int count = 0;
            Vector3 dst = data(*vit).c2;
            double light = GetLight(dst);
            int vsmall = 0;
            int vlight = 0;
            Vector3 sum;
            for(VVIter vv_itr = vv_iter(*vit) ; vv_itr.is_valid(); ++vv_itr)
            {
                count++;
                if(data(*vv_itr).constraint == 0)
                {
                    if(GetLight(data(*vv_itr).c2) < light)
                    {
                        vsmall++;
                    }
                    else
                    {
                        sum += data(*vv_itr).c2;
                        vlight++;
                    }
                }
            }
            if(vlight > 0)
            {
                sum /= vlight;
                data(*vit).c3 = sum;
            }
        }
    }
    for(BasicMesh::VIter vit = vertices_begin(); vit != vertices_end(); ++vit)
    {
        if(data(*vit).constraint == 0 && data(*vit).c3 != Vector3())
        {
            data(*vit).c2 = data(*vit).c3;
        }
    }
}

void PicMesh::MarkColoredFace()
{
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = data(*fit).rid;
        int c = 0;
        int c0 = 0;
        BasicMesh::Point mid = MidPoint(*fit);
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            if(data(*fvit).constraint == 0)
            {
                c0++;
            }
        }
        if(c0 > 0)
        {
            data(*fit).mark = c0;
            if(c0 < 3)
            {
                Vector3 sumc;
                for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
                {
                    if(data(*fvit).constraint == 0)
                    {
                        sumc += data(*fvit).c2;
                    }
                }
                sumc /= c0;
                data(*fit).c2 = sumc;
            }
        }
    }
}

void PicMesh::FillConstraintRegion()
{
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).mark == 0)
        {
            for(FaceHalfedgeIter feit = fh_iter(*fit); feit.is_valid(); ++feit)
            {
                if(data(edge_handle(*feit)).constraint == 0)
                {
                    BasicMesh::HalfedgeHandle _hedge = *feit;
                    FaceHandle fh = opposite_face_handle(_hedge);
                    if(fh.is_valid() && data(fh).mark < 3 && data(fh).mark != 0)
                    {
                        data(*fit).c2 = data(fh).c2;
                        data(*fit).mark = -1;
                        break;
                    }
                }
            }
            if(data(*fit).mark == -1)
            {
                FHandles que;
                for(FaceHalfedgeIter feit = fh_iter(*fit); feit.is_valid(); ++feit)
                {
                    if(data(edge_handle(*feit)).constraint == 0)
                    {
                        BasicMesh::HalfedgeHandle _hedge = *feit;
                        FaceHandle fh = opposite_face_handle(_hedge);
                        if(fh.is_valid() && data(fh).mark == 0)
                        {
                            que.push_back(fh);
                            data(fh).c2 = data(*fit).c2;
                        }
                    }
                }
                while(!que.empty())
                {
                    FHandle now = que.back();
                    que.pop_back();
                    for(FaceHalfedgeIter feit = fh_iter(now); feit.is_valid(); ++feit)
                    {
                        if(edge_handle(*feit).is_valid() &&
                                data(edge_handle(*feit)).constraint == 0)
                        {
                            BasicMesh::HalfedgeHandle _hedge = *feit;
                            FaceHandle fh = opposite_face_handle(_hedge);
                            if(fh.is_valid() && data(fh).mark == 0)
                            {
                                que.push_back(fh);
                                data(fh).c2 = data(*fit).c2;
                                data(fh).mark = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}

void PicMesh::MappingMeshByMidPoint(PicMesh& pm, double x, double y)
{
    BasicMesh::VHandle lastV(pm.n_vertices() / 2);
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
    for(BasicMesh::FIter pfit = faces_begin(); pfit != faces_end(); ++pfit)
    {
        BasicMesh::Point now = MidPoint(*pfit);
        now[0] += x;
        now[1] += y;
        BasicMesh::VHandle lastV(pm.n_vertices() / 2);
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
        int minidx = -1;
        int minidxf = -1;
        mindis = 999999;
        for(VFIter ffit = pm.vf_iter(lastV); ffit.is_valid(); ++ffit)
        {
            Point tp = pm.MidPoint(*ffit);
            double dis = (tp - now).sqrnorm();
            if(dis < mindis)
            {
                mindis = dis;
                minidx = pm.data(*ffit).rid;
                minidxf = (*ffit).idx();
            }
        }
        BasicMesh::FHandle lastF(minidxf);
        data(*pfit).mapid = minidx;
        if(minidxf >= 0)
        {
            data(*pfit).c2 = pm.data(lastF).c2;
        }
    }
    printf("m_Regions.size() %d\n", m_Regions.size());
    m_MapingRegionIDs.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        ints count(pm.m_Regions.size(), 0);
        for(int j = 0; j < m_Regions[i].size(); ++j)
        {
            int mapid = data(m_Regions[i][j]).mapid;
            if(mapid >= 0 && mapid < count.size())
            {
                count[mapid]++;
            }
        }
        int maxcount = 0;
        int maxid = 0;
        for(int j = 0; j < pm.m_Regions.size(); ++j)
        {
            if(count[j] > maxcount)
            {
                maxcount = count[j];
                maxid = j;
            }
        }
        //printf("%d maxid %d\n", i, maxid);
        m_MapingRegionIDs[i] = maxid;
    }
}

void PicMesh::MakeColor7(Vector3s& colors, ints& lastmap)
{
    //printf("m_MapingRegionIDs.size() %d\n", m_MapingRegionIDs.size());
    ints newmaps(m_MapingRegionIDs.size());
    for(int i = 0; i < m_MapingRegionIDs.size(); ++i)
    {
        newmaps[i] = lastmap[m_MapingRegionIDs[i]];
    }
    ColorTriangle t;
    m_Trangles.clear();
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = newmaps[data(*fit).rid];
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(rid >= 0 && rid < colors.size())
            {
                t.color[c] = colors[rid];
            }
            ++c;
        }
        m_Trangles.push_back(t);
    }
    lastmap = newmaps;
}

void PicMesh::MakeColor72(ColorConstraints& colors, ints& lastmap, double x, double y)
{
    //printf("m_MapingRegionIDs.size() %d\n", m_MapingRegionIDs.size());
    ints newmaps(m_MapingRegionIDs.size());
    for(int i = 0; i < m_MapingRegionIDs.size(); ++i)
    {
        newmaps[i] = lastmap[m_MapingRegionIDs[i]];
    }
    ColorTriangle t;
    m_Trangles.clear();
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = newmaps[data(*fit).rid];
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            if(rid >= 0 && rid < colors.size())
            {
                t.color[c] = colors[rid].GetColorVector3(p[0] + x, p[1] + y);
            }
            ++c;
        }
        m_Trangles.push_back(t);
    }
    lastmap = newmaps;
}

void PicMesh::MakeColor8(PicMesh& pm)
{
    //printf("m_MapingRegionIDs.size() %d\n", m_MapingRegionIDs.size());
//  ints newmaps(lastmap.size());
//  for(int i = 0; i < lastmap.size(); ++i)
//  {
//      if(m_MapingRegionIDs.size() > lastmap[i])
//      {
//          newmaps[i] = m_MapingRegionIDs[lastmap[i]];
//          printf("newmaps[%d] %d old %d last %d\n", i, newmaps[i], m_MapingRegionIDs[lastmap[i]], lastmap[i]);
//      }
//  }
    ColorTriangle t;
    m_Trangles.clear();
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int rid = data(*fit).mapid;
        Vector3 cc = pm.data(FHandle(rid)).c2;
        data(*fit).c2 = cc;
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            t.color[c] = cc;
            ++c;
        }
        m_Trangles.push_back(t);
    }
    //lastmap = newmaps;
}

void PicMesh::MakeColor9()
{
    ColorTriangle t;
    m_Trangles.clear();
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        int c = 0;
        for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
        {
            Point p = point(*fvit);
            t.pts[c][0] = p[0];
            t.pts[c][1] = p[1];
            t.color[c] = data(*fit).c2;
            ++c;
        }
        m_Trangles.push_back(t);
    }
}

void PicMesh::ComputeRegion()
{
    m_RegionAreas.resize(m_Regions.size());
    m_RegionRank.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        m_RegionAreas[i] = m_Regions[i].size();
    }
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        for(int j = 0; j < m_Regions.size(); ++j)
        {
            if(m_RegionAreas[i] < m_RegionAreas[j])
            {
                m_RegionRank[i]++;
            }
        }
    }
    m_RegionAABB.clear();
    m_RegionAABB.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        FHandles& region = m_Regions[i];
        for(int j = 0; j < region.size(); ++j)
        {
            for(FVIter fvit = fv_iter(region[j]); fvit.is_valid(); ++fvit)
            {
                Point p = point(*fvit);
                m_RegionAABB[i].AddPoint(Vector2(p[0], p[1]));
            }
        }
    }
}

void PicMesh::ComputeNeighbor()
{
    m_RegionNeighbor.clear();
    m_RegionNeighbor.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        std::set<int> intsetv;
        for(int j = 0; j < m_Regions[i].size(); ++j)
        {
            for(FFIter ffit = ff_iter(m_Regions[i][j]); ffit.is_valid(); ++ffit)
            {
                if(data(*ffit).rid != i)
                {
                    intsetv.insert(data(*ffit).rid);
                }
            }
        }
        for(auto it = intsetv.begin(); it != intsetv.end(); ++it)
        {
            m_RegionNeighbor[i].push_back(*it);
        }
    }
}

char PicMesh::bindnode(chars& used, int idx)
{
    if(used[idx])
    {
        return 0;
    }
    bnode res;
    int maxid = -1;
    const int CONSTANT = 15;
    double maxvalue = CONSTANT;
    for(int i = 0; i < m_RegionNeighbor[idx].size(); ++i)
    {
        if(used[m_RegionNeighbor[idx][i]] == 0)
        {
            //float dis = m_RegionAABB[idx].Distance(m_RegionAABB[m_RegionNeighbor[idx][i]]);
            float dis = m_RegionColor[idx].distance(m_RegionColor[m_RegionNeighbor[idx][i]]);
            if(dis < maxvalue)
            {
                maxid = m_RegionNeighbor[idx][i];
                maxvalue = dis;
            }
        }
    }
    if(maxid != -1)
    {
        // 新增兒子
        res.childs.push_back(idx);
        res.childs.push_back(maxid);
        res.id = m_btree.m_nodes.size();
        // 新增已使用
        used.push_back(0);
        // 新增鄰居
        ints newNeighbor;
        newNeighbor.insert(newNeighbor.end(), m_RegionNeighbor[idx].begin(), m_RegionNeighbor[idx].end());
        newNeighbor.insert(newNeighbor.end(), m_RegionNeighbor[maxid].begin(), m_RegionNeighbor[maxid].end());
        std::sort(newNeighbor.begin(), newNeighbor.end());
        ints::iterator it;
        it = std::unique(newNeighbor.begin(), newNeighbor.end());
        newNeighbor.resize(std::distance(newNeighbor.begin(), it));
        m_RegionNeighbor.push_back(newNeighbor);
        for(int i = 0; i < newNeighbor.size(); ++i)
        {
            m_RegionNeighbor[newNeighbor[i]].push_back(res.id);
        }
        // 新增AABB
        AABB2D aabb;
        aabb.AddPoint(m_RegionAABB[idx].m_Min);
        aabb.AddPoint(m_RegionAABB[idx].m_Max);
        aabb.AddPoint(m_RegionAABB[maxid].m_Min);
        aabb.AddPoint(m_RegionAABB[maxid].m_Max);
        m_RegionAABB.push_back(aabb);
        // 新增面積
        m_RegionAreas.push_back(m_RegionAreas[idx] + m_RegionAreas[maxid]);
        // 新增rank
        m_RegionRank.push_back((m_RegionRank[idx] + m_RegionRank[maxid]) / 2);
        // 新增顏色
        m_RegionColor.push_back((m_RegionColor[idx] + m_RegionColor[maxid]) / 2);
        m_btree.m_nodes.push_back(res);
        MarkTreeLeave(idx);
        MarkTreeLeave(maxid);
    }
    else if(m_RegionNeighbor[idx].size() > 0)
    {
        maxvalue = CONSTANT;
        for(int i = 0; i < m_RegionNeighbor[idx].size(); ++i)
        {
            float dis = m_RegionColor[idx].distance(m_RegionColor[m_RegionNeighbor[idx][i]]);
            if(dis > maxvalue)
            {
                maxid = m_RegionNeighbor[idx][i];
                maxvalue = dis;
            }
        }
        if(maxid != -1)
        {
            // 新增鄰居
            ints& neighbor = m_RegionNeighbor[maxid];
            neighbor.insert(neighbor.begin(),
                            m_RegionNeighbor[idx].begin(),
                            m_RegionNeighbor[idx].end());
            std::sort(neighbor.begin(), neighbor.end());
            ints::iterator it;
            it = std::unique(neighbor.begin(), neighbor.end());
            neighbor.resize(std::distance(neighbor.begin(), it));
            // 新增兒子
            m_btree.m_nodes[maxid].childs.push_back(idx);
            // 新增面積
            m_RegionAreas[maxid] += m_RegionAreas[idx];
            // 新增已使用
            int n = 0;
            while(used[idx] < used[maxid] + 1 && n < 20)
            {
                n++;
                MarkTreeLeave(idx);
            }
        }
    }
}

void PicMesh::BuildBtree()
{
    chars& used = m_used;
    used.clear();
    used.resize(m_Regions.size(), 0);
    m_btree.m_nodes.resize(m_Regions.size());
    std::vector<sortu> tary;
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        m_btree.m_nodes[i].id = i;
    }
    makeRank(tary, used);
    for(int i = 0; i < tary.size() / 4; ++i)
    {
        bindnode(used, tary[i].oid);
    }
    printf("used %d ", used.size());

    makeRank(tary, used);
    for(int i = 0; i < tary.size() / 3; ++i)
    {
        bindnode(used, tary[i].oid);
    }
    printf("used %d ", used.size());
    makeRank(tary, used);
    for(int i = 0; i < tary.size() / 2; ++i)
    {
        bindnode(used, tary[i].oid);
    }
    printf("used %d ", used.size());
    makeRank(tary, used);
    for(int i = 0; i < tary.size(); ++i)
    {
        bindnode(used, tary[i].oid);
    }
//     printf("used %d \n", used.size());
//     makeRank(tary, used);
//     for(int i = 0; i < tary.size(); ++i)
//     {
//         bindnode(used, tary[i].oid);
//     }
//     printf("used %d \n", used.size());
//     makeRank(tary, used);
//     for(int i = 0; i < tary.size(); ++i)
//     {
//         bindnode(used, tary[i].oid);
//     }
//     printf("used %d \n", used.size());
//     makeRank(tary, used);
}

void PicMesh::MakeColorX1(Vector3s& colors)
{
    m_Trangles.clear();
    ColorTriangle t;
    for(int i = 0; i < m_used.size(); ++i)
    {
        printf("m_used[%d] %d\n", i, m_used[i]);
        if(m_used[i] <= 0)
        {
            MakeTreeLeaveColor(i, colors[i % colors.size()]);
        }
    }
}

void PicMesh::MarkTreeLeave(int idx)
{
    std::set<int> isused;
    ints ids;
    ids.push_back(idx);
    for(; ids.size() > 0;)
    {
        int id = ids.back();
        ids.pop_back();
        isused.insert(id);
        ints& nodes = m_btree.m_nodes[id].childs;
        for(int i = 0; i < nodes.size(); ++i)
        {
            bool has = false;
            if(isused.find(nodes[i]) != isused.end())
            {
                has = true;
            }
            else if(std::find(ids.begin(), ids.end(), nodes[i]) != ids.end())
            {
                has = true;
            }
            if(!has)
            {
                ids.push_back(nodes[i]);
            }
        }
    }
    for(auto it = isused.begin(); it != isused.end(); ++it)
    {
        m_used[*it]++;
    }
}

void PicMesh::makeRank(std::vector<sortu>& tary, chars& used)
{
    tary.resize(used.size());
    for(int i = 0; i < used.size(); ++i)
    {
        tary[i].oid = i;
        tary[i].size = m_RegionAreas[i];
    }
    std::sort(tary.begin(), tary.end());
}

void PicMesh::MakeTreeLeaveColor(int idx, Vector3 colors)
{
    ColorTriangle t;
    std::set<int> isused;
    ints ids;
    ids.push_back(idx);
    for(; ids.size() > 0;)
    {
        int id = ids.back();
        ids.pop_back();
        isused.insert(id);
        ints& nodes = m_btree.m_nodes[id].childs;
        for(int i = 0; i < nodes.size(); ++i)
        {
            bool has = false;
            if(isused.find(nodes[i]) != isused.end())
            {
                has = true;
            }
            else if(std::find(ids.begin(), ids.end(), nodes[i]) != ids.end())
            {
                has = true;
            }
            if(!has)
            {
                ids.push_back(nodes[i]);
            }
        }
    }
    for(auto it = isused.begin(); it != isused.end(); ++it)
    {
        if(*it < m_Regions.size())
        {
            for(int j = 0; j < m_Regions[*it].size(); ++j)
            {
                int c = 0;
                for(FVIter fvit = fv_iter(m_Regions[*it][j]); fvit.is_valid(); ++fvit)
                {
                    Point p = point(*fvit);
                    t.pts[c][0] = p[0];
                    t.pts[c][1] = p[1];
                    t.color[c] = colors / (m_used[*it]);
                    ++c;
                }
                m_Trangles.push_back(t);
            }
        }
    }
}

void PicMesh::SetRegionColor(cv::Mat& img)
{
    vavImage vimg(img);
    m_RegionColor.resize(m_Regions.size());
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        if(m_Regions[i].size() > 1)
        {
            Vector3 nc;
            float scalar = 1.f / 30.f;
            std::set<int> intsetv;
            float step = (m_Regions[i].size() - 1) / 30.f;
            for(float j = 0; j < m_Regions[i].size(); j += step)
            {
                Point p = MidPoint(m_Regions[i][(int)j]);
                p[0] += 0.5;
                p[1] += 0.5;
                nc += scalar * vimg.GetBilinearColor(p[0], p[1]);
            }
            m_RegionColor[i] = nc;
        }
        else
        {
            if(m_Regions[i].size() != 0)
            {
                Point p = MidPoint(m_Regions[i][0]);
                p[0] += 0.5;
                p[1] += 0.5;
                m_RegionColor[i] = vimg.GetBilinearColor(p[0], p[1]);
            }
        }
    }
}

void PicMesh::DrawTree()
{
    m_RegionSize.resize(m_btree.m_nodes.size());
    int sum = 0;
    int id = 0;
    for(int i = 0; i < m_RegionSize.size(); ++i)
    {
        m_RegionSize[i] = GetLeaveSize(i);
        if(m_used[i] <= 0)
        {
            sum += m_RegionSize[i];
        }
    }
    printf("tree sum: %d\n", sum);
    cv::Mat tree;
    tree.create(800, sum * 4, CV_8UC3);
    tree = cv::Scalar(0);
    int drawright = 0;
    for(int i = 0; i < m_RegionSize.size(); ++i)
    {
        if(m_used[i] == 0)
        {

            DrawLeave(i, tree, drawright, 5);
            drawright += m_RegionSize[i] * 3 + 5;
        }
    }
    static int xx = 0;
    xx++;
    char tmppath[100];
    sprintf(tmppath, "tree_%04d.png", xx);
    cv::imwrite(tmppath, tree);
}

int PicMesh::GetLeaveSize(int idx)
{
    std::set<int> isused;
    ints ids;
    ids.push_back(idx);
    for(; ids.size() > 0;)
    {
        int id = ids.back();
        ids.pop_back();
        isused.insert(id);
        ints& nodes = m_btree.m_nodes[id].childs;
        for(int i = 0; i < nodes.size(); ++i)
        {
            bool has = false;
            if(isused.find(nodes[i]) != isused.end())
            {
                has = true;
            }
            else if(std::find(ids.begin(), ids.end(), nodes[i]) != ids.end())
            {
                has = true;
            }
            if(!has)
            {
                ids.push_back(nodes[i]);
            }
        }
    }
    return isused.size();
}

void PicMesh::DrawLeave(int idx, cv::Mat& img, int x, int y)
{
    int width = m_RegionSize[idx];
    ints& nodes = m_btree.m_nodes[idx].childs;
    if(nodes.size() == 0)
    {
        return;
    }
    int childsize = width * 1.5 / nodes.size();
    if(childsize > 25)
    {
        childsize = 25;
    }
    if(childsize < 4)
    {
        childsize = 4;
    }
    for(int i = 0; i < nodes.size(); ++i)
    {
        cv::line(img, cv::Point(x, y), cv::Point(x + childsize * i, y + 20), cv::Scalar(255, 255, 255));;
        if(m_btree.m_nodes[nodes[i]].childs.size() > 0)
        {
            DrawLeave(nodes[i], img, x + childsize * i, y + 20);
        }
    }
}

void PicMesh::MakeColorX9(int id)
{
    ColorTriangle t;
    m_Trangles.clear();
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(data(*fit).rid == id)
        {
            int c = 0;
            for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
            {
                Point p = point(*fvit);
                t.pts[c][0] = p[0];
                t.pts[c][1] = p[1];
                t.color[c] = data(*fit).c2;
                ++c;
            }
            m_Trangles.push_back(t);
        }
    }
}

void PicMesh::MakeColorX2(ints& mappings, float x, float y)
{
    ColorTriangle t;
    m_Trangles.clear();
    chars marks(m_Regions.size());
    for(int i = 0; i < mappings.size(); ++i)
    {
        marks[mappings[i]] = 1;
    }
    for(FIter fit = faces_begin(); fit != faces_end(); ++fit)
    {
        if(marks[data(*fit).rid] != 1)
        {
            int c = 0;
            for(FVIter fvit = fv_iter(*fit); fvit.is_valid(); ++fvit)
            {
                Point p = point(*fvit);
                t.pts[c][0] = p[0] + x;
                t.pts[c][1] = p[1] + y;
                t.color[c] = data(*fit).c2;
                ++c;
            }
            m_Trangles.push_back(t);
        }
    }
}

int PicMesh::GetRegionId(float x, float y)
{
    BasicMesh::VHandle lastV(n_vertices() / 2);
    BasicMesh::Point now(x , y);
    BasicMesh::FHandle dst(-1);
    double tdis = (point(lastV) - now).sqrnorm();
    double mindis = tdis;
    for(; tdis > 1;)
    {
        int minidx = -1;
        for(VVIter vvit = vv_iter(lastV); vvit.is_valid(); ++vvit)
        {
            Point tp = point(*vvit);;
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
    for(VFIter vfit = vf_iter(lastV); vfit.is_valid(); ++vfit)
    {
        double dis = (MidPoint(*vfit) - now).sqrnorm();
        if(dis < mindis)
        {
            mindis = dis;
            dst = *vfit;
        }
    }

    if(dst.idx() >= 0)
    {
        printf("mapping to key frame index %d \n", data(dst).rid2);
        return data(dst).rid2;
    }
    return -1;
}




