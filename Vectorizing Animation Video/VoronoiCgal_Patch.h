#pragma once
#ifdef USE_CGAL
#include <iostream>
#include "TriangulationBase.h"
#pragma warning( push, 0 )
// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#pragma warning( pop ) 

#include "ImageSpline.h"
#include "LineDef.h"
#include "CgalPatch.h"
#include "PositionGraph.h"

class VoronoiCgal_Patch : public TriangulationBase
{
public:
	static const int TRIANGLE_NOT_INIT = -1;
	static const int TRIANGLE_TRANSPARENT = -2;

	struct FaceInfo2
	{
		FaceInfo2(): nesting_level(TRIANGLE_NOT_INIT)
		{
		}
		int nesting_level;
		bool in_domain()
		{
			//return nesting_level %3 == 1;
			return nesting_level >= 0;
		}
	};

	struct VertexInfo2
	{
		VertexInfo2(): nesting_level(TRIANGLE_NOT_INIT) {}
		int nesting_level;
	};
	typedef CGAL::Triangulation_vertex_base_with_info_2<VertexInfo2, CgalInexactKernel> Vb;
	typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, CgalInexactKernel> Fbbb;
	typedef CGAL::Constrained_triangulation_face_base_2<CgalInexactKernel, Fbbb>    Fbb;
	typedef CGAL::Delaunay_mesh_face_base_2<CgalInexactKernel, Fbb>         Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb>            TDS;
	typedef CGAL::Exact_predicates_tag                              Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<CgalInexactKernel, TDS, Itag> Delaunay;

//  typedef CGAL::Triangulation_data_structure_2< CGAL::Triangulation_vertex_base_2<K>, CGAL::Triangulation_face_base_with_info_2<VFaceInfo2, K> > myface;
//  typedef CGAL::Constrained_Delaunay_triangulation_2<K, myface> Delaunay;
	typedef Delaunay::Vertex_handle Vertex_handle;
	typedef CGAL::Delaunay_mesh_size_criteria_2<Delaunay>   Criteria;
	typedef CGAL::Delaunay_mesher_2<Delaunay, Criteria> Mesher;

	//typedef std::vector<Site>     Sites;
	typedef std::vector<Vertex_handle>  Vertex_handles;

	LineSegs    m_LineSegs;
	Lines       m_Lines;
	Lines       m_Controls;
	CgalLine      m_Points;
	Delaunay    m_Delaunay;
	ImageSpline m_ImageSpline;
	CgalPatchs  m_CgalPatchs;
	CgalLines    m_OutLines;
	PositionGraph   m_PositionGraph;

	void MakeGraphLines();

	void insert_polygon(Delaunay& cdt, ImageSpline& m_ImageSpline, int idx);

	void insert_polygonInter(Delaunay& cdt, ImageSpline& m_ImageSpline, int idx);
	void insert_polygonInter2(Delaunay& cdt, ImageSpline& m_ImageSpline,
	                          PatchSpline& ps);

	void AddImageSpline(ImageSpline& is)
	{
		m_ImageSpline = is;
	}
	void AddPoint(double x, double y)
	{
		m_Points.push_back(CgalPoint(x, y));
	}
	void Compute();
	void Clear() {}

	void    mark_domains(Delaunay& ct, Delaunay::Face_handle start, int index,
	                     std::list<Delaunay::Edge>& border);

	void    mark_domains(Delaunay& cdt);

};

#endif // USE_CGAL
