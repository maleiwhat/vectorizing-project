#include <auto_link_cgal4.hpp>
#include "TriangulationCgal.h"
#include <auto_link_mpir.hpp>
#include <auto_link_mpfr.hpp>
#include <boost/chrono/time_point.hpp>
#include <iostream>

void TriangulationCgal_SeedPoint::AddPoint(float x, float y)
{
	m_Triangulation.insert(Point(x, y));
}

void TriangulationCgal_SeedPoint::Compute()
{
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
		t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
		t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
		m_Triangles.push_back(t);
	}
}

void TriangulationCgal_SeedPoint::Clear()
{
	m_Triangles.clear();
}

void TriangulationCgal_Sideline::AddPoint(float x, float y)
{
	m_Points.push_back(Point(x, y));
}

void TriangulationCgal_Sideline::AddSeedPoint(float x, float y)
{
	m_SeedPoints.push_back(Point(x, y));
}

void TriangulationCgal_Sideline::Compute()
{
	if (m_Points.size() < 10) { return; }

	m_Triangulation.clear();
	Vertex_handles vhs;

//  for (int i=0;i<m_Points.size();++i)
//  {
//      vhs.push_back(m_Triangulation.insert( m_Points[i] ));
//  }
//  for (int i=m_Points.size()-1;i>0;--i)
//  {
//      m_Triangulation.insert_constraint(m_Points[i], m_Points[i-1]);
//  }
//  m_Triangulation.insert_constraint(m_Points.front(), m_Points.back());
	if (!m_SeedPoints.empty())
	{
		for (int i = 0; i < m_SeedPoints.size() - 1; ++i)
		{
			m_Triangulation.insert(m_SeedPoints[i]);
		}
	}

	for (int i = 0; i < m_Points.size() - 1; ++i)
	{
		m_Triangulation.insert_constraint(m_Points[i], m_Points[i + 1]);
	}

	m_Triangulation.insert_constraint(m_Points.back(), m_Points.front());
	//insert_polygon(m_Triangulation, m_Polygon);
	//mark_domains(m_Triangulation);
	Mesher mesher(m_Triangulation);
	mesher.set_criteria(m_Criteria);
	mesher.refine_mesh();
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for (; fc != m_Triangulation.finite_faces_end(); ++fc)
	{
		if (fc->is_in_domain())
		{
			t.m_Points[0] = Vector2(fc->vertex(0)->point()[0], fc->vertex(0)->point()[1]);
			t.m_Points[1] = Vector2(fc->vertex(1)->point()[0], fc->vertex(1)->point()[1]);
			t.m_Points[2] = Vector2(fc->vertex(2)->point()[0], fc->vertex(2)->point()[1]);
			m_Triangles.push_back(t);
		}
	}
}

void TriangulationCgal_Sideline::SetCriteria(float shapebound, float length)
{
	m_Criteria = Criteria(shapebound, length);
}

void TriangulationCgal_Sideline::Clear()
{
	m_Points.clear();
	m_SeedPoints.clear();
}

void TriangulationCgal_Sideline::insert_polygon(Triangulation& cdt,
        const Polygon_2& polygon)
{
	if (polygon.is_empty()) { return; }

	Triangulation::Vertex_handle v_prev = cdt.insert(*CGAL::cpp0x::prev(
	        polygon.vertices_end()));

	for (Polygon_2::Vertex_iterator vit = polygon.vertices_begin();
	        vit != polygon.vertices_end(); ++vit)
	{
		Triangulation::Vertex_handle vh = cdt.insert(*vit);
		cdt.insert_constraint(vh, v_prev);
		v_prev = vh;
	}
}

