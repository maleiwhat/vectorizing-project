#include "TriangulationCgal_Sideline.h"


void TriangulationCgal_Sideline::AddPoint( double x, double y )
{
	m_Points.push_back( Point( x, y ) );
}

void TriangulationCgal_Sideline::AddSeedPoint( double x, double y )
{
	m_SeedPoints.push_back( Point( x, y ) );
}

void TriangulationCgal_Sideline::Compute()
{
	if (m_Points.size() < 10) return;
	m_Triangulation.clear();
	Vertex_handles vhs;
	if (!m_SeedPoints.empty())
	{
		for (int i=0;i<m_SeedPoints.size()-1;++i)
		{
			m_Triangulation.insert(m_SeedPoints[i]);
		}
	}

	for (int i=0;i<m_Points.size()-1;++i)
	{
		m_Triangulation.insert_constraint(m_Points[i], m_Points[i+1]);
	}
	m_Triangulation.insert_constraint(m_Points.back(), m_Points.front());

	Mesher mesher(m_Triangulation);
	mesher.set_criteria(m_Criteria);
	mesher.refine_mesh();
	m_Triangles.clear();
	Triangle t;
	Finite_faces_iterator fc = m_Triangulation.finite_faces_begin();

	for ( ; fc != m_Triangulation.finite_faces_end(); ++fc )
	{
		if (fc->is_in_domain())
		{
			t.m_Points[0] = Vector2( fc->vertex( 0 )->point()[0], fc->vertex( 0 )->point()[1] );
			t.m_Points[1] = Vector2( fc->vertex( 1 )->point()[0], fc->vertex( 1 )->point()[1] );
			t.m_Points[2] = Vector2( fc->vertex( 2 )->point()[0], fc->vertex( 2 )->point()[1] );
			m_Triangles.push_back( t );
		}
	}
}

void TriangulationCgal_Sideline::SetCriteria( float shapebound, float length )
{
	m_Criteria = Criteria( shapebound, length );
}

void TriangulationCgal_Sideline::Clear()
{
	m_Points.clear();
	m_SeedPoints.clear();
}


