#include <auto_link_cgal4.hpp>
#include <auto_link_mpir.hpp>
#include <auto_link_mpfr.hpp>

#include "TriangulationCgal_SeedPoint.h"
#include <boost/chrono/time_point.hpp>
#include <iostream>

void TriangulationCgal_SeedPoint::AddPoint( double x, double y )
{
	m_Triangulation.insert( Point( x, y ) );
}

void TriangulationCgal_SeedPoint::Compute()
{
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for ( ; fc != m_Triangulation.finite_faces_end(); ++fc )
	{
		t.m_Points[0] = Vector2( fc->vertex( 0 )->point()[0], fc->vertex( 0 )->point()[1] );
		t.m_Points[1] = Vector2( fc->vertex( 1 )->point()[0], fc->vertex( 1 )->point()[1] );
		t.m_Points[2] = Vector2( fc->vertex( 2 )->point()[0], fc->vertex( 2 )->point()[1] );
		m_Triangles.push_back( t );
	}
}

void TriangulationCgal_SeedPoint::Clear()
{
	m_Triangles.clear();
}

