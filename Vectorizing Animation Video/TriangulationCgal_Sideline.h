#pragma once
#include "TriangulationBase.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Polygon_2.h>

#include "Patch.h"
#include "ColorConstraint.h"
#include "ImageSpline.h"
#include "PositionGraph.h"
#include "Line.h"



class TriangulationCgal_Sideline : public TriangulationBase
{
public:
	struct FaceInfo2
	{
		FaceInfo2(): nesting_level(-1), color_label(0) {}
		int nesting_level;
		int color_label;
		bool in_domain()
		{
			return nesting_level >= 0;
		}
		bool in_domain2()
		{
			return nesting_level % 2 == 1;
		}

	};

	struct VertexInfo2
	{
		VertexInfo2(): nesting_level(-1) {}
		int nesting_level;
	};
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	//typedef CGAL::Triangulation_vertex_base_2<K>                    Vb;
	typedef CGAL::Triangulation_vertex_base_with_info_2<VertexInfo2, K> Vb;
	typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K> Fbbb;
	typedef CGAL::Constrained_triangulation_face_base_2<K, Fbbb>    Fbb;
	typedef CGAL::Delaunay_mesh_face_base_2<K, Fbb>         Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb>            TDS;
	typedef CGAL::Exact_predicates_tag                              Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> Triangulation;
	typedef CGAL::Delaunay_mesh_size_criteria_2<Triangulation>  Criteria;
	typedef CGAL::Delaunay_mesher_2<Triangulation, Criteria>    Mesher;
	typedef CGAL::Polygon_2<K>                  Polygon;

	typedef Triangulation::Face_handle          Face_handle;
	typedef Triangulation::Finite_faces_iterator        Finite_faces_iterator;
	typedef Triangulation::Finite_vertices_iterator     Finite_vertices_iterator;
	typedef Triangulation::Vertex_handle            Vertex_handle;
	typedef Triangulation::Point                Point;

	typedef std::vector<Point>      Points;
	typedef std::vector<Vertex_handle>  Vertex_handles;

	void    AddColorConstraint(ColorConstraint_sptr constraint);
	void    AddPatch(Patch& cvps);
	void    AddImageSpline(ImageSpline& is);
	TriangulationCgal_Sideline(): m_Criteria(0.125, 5) {}
	virtual ~TriangulationCgal_Sideline() {}
	virtual void AddPoint(double x, double y);
	virtual void Compute();
	virtual void Clear();
	void    MakeColorSequential()
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
	void    MakeColorRandom()
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
	void    SetSize(int w, int h);
	void    SetCriteria(float shapebound, float length);
	void    insert_polygon(Triangulation& cdt, const Polygon& polygon, int domain);

	void mark_domains(Triangulation& ct, Triangulation::Face_handle start,
	                  int index, std::list<Triangulation::Edge>& border);
	void mark_domains(Triangulation& cdt);
	void mark_domains2(Triangulation& ct, Triangulation::Face_handle start,
	                   int index, std::list<Triangulation::Edge>& border, int color_label);
	void mark_domains2(Triangulation& cdt);
	void insert_polygon(Triangulation& cdt, ImageSpline& is, int i);
	void insert_polygonInter(Triangulation& cdt, ImageSpline& is, int idx);
	void insert_polygonInter2(Triangulation& cdt, ImageSpline& is, PatchSpline& ps);
	static const int TRIANGLE_NOT_INIT = -1;
	static const int TRIANGLE_TRANSPARENT = -2;


	Triangulation   m_Triangulation;
	Criteria    m_Criteria;
	Points      m_SeedPoints;
	Patchs      m_Patch;
	Polygon     m_Polygon;
	int     m_w, m_h;
	ColorConstraint_sptrs   m_ColorConstraint;
	ImageSpline m_ImageSpline;
	PositionGraph   m_PositionGraph;
	LineSegs    m_LineSegs;
	Lines       m_OriginLines;
	Lines       m_Lines;
	double_vector2d m_LinesWidth;
	Lines       m_Controls;
	Lines       m_ContourLines;
};


