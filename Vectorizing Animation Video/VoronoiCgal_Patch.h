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

static const int TRIANGLE_NOT_INIT = -1;
static const int TRIANGLE_TRANSPARENT = -2;

struct VFaceInfo2
{
	VFaceInfo2():nesting_level(TRIANGLE_NOT_INIT) 
	{
		memset(edge, 0, sizeof(edge));
	}
	int nesting_level;
	int edge[3];
	bool in_domain()
	{
		//return nesting_level %3 == 1;
		return nesting_level >= 0;
	}
};

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
	Lines		m_Controls;
	Points		m_Points;
	Delaunay	m_Delaunay;
	ImageSpline	m_ImageSpline;
	CgalPatchs	m_CgalPatchs;
	Points2d	m_OutLines;
	PositionGraph	m_PositionGraph;

	void MakeGraphLines();

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


	bool	mark_domains(int idx, Delaunay::Face_handle start, std::list<Delaunay::Edge>& border, Line& line);
	
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
				//mark_domains(idx, fc, border, line);
				//m_Lines.push_back(line);
				//break;
			}
		}

// 		while (! border.empty())
// 		{
// 			Delaunay::Edge e = border.front();
// 			border.pop_front();
// 			Delaunay::Face_handle n = e.first->neighbor(e.second);
// 
// 			if (n->info().nesting_level == TRIANGLE_NOT_INIT)
// 			{
// 				Line line;
// 				//mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
// 				mark_domains(idx, n, border, line);
// 				m_Lines.push_back(line);
// 			}
// 		}
	}

};




