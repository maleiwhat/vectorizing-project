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
	virtual void AddPoint( float x, float y );
	virtual void Compute();
	virtual void Clear();

	Triangulation	m_Triangulation;
};

class TriangulationCgal_Sideline : public TriangulationBase
{
public:
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Triangulation_vertex_base_2<K> Vb;
	typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
	typedef CGAL::Exact_predicates_tag                                Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds, Itag> Triangulation;
	typedef CGAL::Delaunay_mesh_size_criteria_2<Triangulation> Criteria;
	typedef CGAL::Delaunay_mesher_2<Triangulation, Criteria> Mesher;
	typedef CGAL::Polygon_2<K> Polygon_2;

	typedef Triangulation::Face_handle Face_handle;
	typedef Triangulation::Finite_faces_iterator Finite_faces_iterator;
	typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
	typedef Triangulation::Vertex_handle Vertex_handle;
	typedef Triangulation::Point Point;
	typedef std::vector<Point> Points;
	typedef std::vector<Vertex_handle> Vertex_handles;
	

	TriangulationCgal_Sideline(): m_Criteria( 0.125, 5 ) {}
	virtual ~TriangulationCgal_Sideline() {}
	virtual void AddPoint( float x, float y );
	virtual void AddSeedPoint( float x, float y );
	virtual void Compute();
	virtual void Clear();
	void	SetCriteria( float shapebound, float length );
	void	insert_polygon(Triangulation& cdt,const Polygon_2& polygon);

	void mark_domains( Triangulation& ct, Triangulation::Face_handle start, int index, std::list<Triangulation::Edge>& border );
	void mark_domains( Triangulation& cdt );

	Triangulation	m_Triangulation;
	Criteria	m_Criteria;
	Points		m_Points;
	Points		m_SeedPoints;
	Polygon_2	m_Polygon;
};
