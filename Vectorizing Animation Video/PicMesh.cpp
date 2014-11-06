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
                printf("-");
            }
            if(mindis < initdis)
            {
                printf("dis: %f\n", mindis);
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
			int maxid = it-count.begin();
			for(int j = 0; j < m_Regions[i].size(); ++j)
			{
				data(m_Regions[i][j]).cid = rnum[maxid];
			}
        }
    }
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




