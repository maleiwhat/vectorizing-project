#pragma once
#include "TriangulationBase.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Polygon_2.h>

class TriangulationCgal_SeedPoint : public TriangulationBase
{
public:
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

	typedef CGAL::Triangulation_vertex_base_2<K> Vb;
	typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> Triangulation;

	typedef Triangulation::Face_handle Face_handle;
	typedef Triangulation::Finite_faces_iterator Finite_faces_iterator;
	typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
	typedef Triangulation::Point  Point;

	virtual ~TriangulationCgal_SeedPoint() {}
	virtual void AddPoint(double x, double y);
	virtual void Compute();
	virtual void Clear();

	Triangulation   m_Triangulation;
};

