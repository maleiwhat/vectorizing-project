#pragma once
#include <iostream>
#include "TriangulationBase.h"

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

#include "ImageSpline.h"
#include "Line.h"
#include "CgalPatch.h"
#include "PositionGraph.h"

struct VFaceInfo2
{
	VFaceInfo2() {}
	int nesting_level;
	bool in_domain()
	{
		//return nesting_level %3 == 1;
		return nesting_level >= 0;
	}
};

static const int TRIANGLE_NOT_INIT = -1;
static const int TRIANGLE_TRANSPARENT = -2;
class VoronoiCgal_Patch : public TriangulationBase
{
public:
	typedef CGAL::Triangulation_data_structure_2< CGAL::Triangulation_vertex_base_2<K>, CGAL::Triangulation_face_base_with_info_2<VFaceInfo2, K> > myface;
	typedef CGAL::Delaunay_triangulation_2<K, myface> Delaunay;
	typedef Delaunay::Vertex_handle Vertex_handle;

	//typedef std::vector<Site>		Sites;
	typedef std::vector<Vertex_handle>	Vertex_handles;

	LineSegs	m_LineSegs;
	Lines		m_Lines;
	Points		m_Points;
	Delaunay	m_Delaunay;
	ImageSpline	m_ImageSpline;
	CgalPatchs	m_CgalPatchs;
	Points2d	m_OutLines;
	PositionGraph	m_PositionGraph;


	void MakeLines();

	void insert_polygon(Delaunay& cdt, ImageSpline& m_ImageSpline, int idx);

	void insert_polygonInter(Delaunay& cdt, ImageSpline& m_ImageSpline, int idx);
	void insert_polygonInter2(Delaunay& cdt, ImageSpline& m_ImageSpline, PatchSpline& ps);

	void AddImageSpline(ImageSpline& is)
	{
		m_ImageSpline = is;
	}
	void AddPoint(double x, double y)
	{
		m_Points.push_back(Point(x, y));
	}
	void Compute();
	void Clear() {}


	void	mark_domains(int idx, Delaunay::Face_handle start, std::list<Delaunay::Edge>& border, Line& line)
	{
		if (start->info().nesting_level != TRIANGLE_NOT_INIT)
		{
			return;
		}

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

					if (m_Delaunay.is_infinite(e))
					{
						continue;
					}

					if (!dont_add && n->info().nesting_level == TRIANGLE_NOT_INIT)
					{
						border.push_back(e);
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
						// show repeat amount
// 						for (int i = 0; i < score.size(); ++i)
// 						{
// 							if (score[i] == 4)
// 							{
// 								std::swap(pts[i], pts[0]);
// 							}
// 
// 							printf("score: %d\n", score[i]);
// 						}
// 
// 						printf("\n");
						m_PositionGraph.AddJoint(pts[0], pts[1], pts[2], pts[3]);
					}
					else
					{
						for (auto it = m_LineSegs.begin(); it != m_LineSegs.end(); ++it)
						{
							m_PositionGraph.AddNewLine(it->beg, it->end);
						}
					}
				}
			}
		}
	}


	void	mark_domains(int idx)
	{
		for (Delaunay::All_faces_iterator it = m_Delaunay.all_faces_begin(); it != m_Delaunay.all_faces_end(); ++it)
		{
			it->info().nesting_level = TRIANGLE_NOT_INIT;
		}

		int index = 0;
		std::list<Delaunay::Edge> border;
		Delaunay::Finite_faces_iterator fc = m_Delaunay.finite_faces_begin();

		for (; fc != m_Delaunay.finite_faces_end(); ++fc)
		{
			if (TRIANGLE_NOT_INIT == fc->info().nesting_level)
			{
				Line line;
				mark_domains(idx, fc, border, line);
				m_Lines.push_back(line);
				break;
			}
		}

		while (! border.empty())
		{
			Delaunay::Edge e = border.front();
			border.pop_front();
			Delaunay::Face_handle n = e.first->neighbor(e.second);

			if (n->info().nesting_level == TRIANGLE_NOT_INIT)
			{
				Line line;
				//mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
				mark_domains(idx, n, border, line);
				m_Lines.push_back(line);
			}
		}
	}

};




