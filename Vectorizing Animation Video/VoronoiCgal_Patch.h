#pragma once
#include <iostream>
#include "TriangulationBase.h"

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>


#include "Line.h"

class VoronoiCgal_Patch : public TriangulationBase
{
public:

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
	typedef Delaunay::Vertex_handle Vertex_handle;

	typedef K::Point_2 Point;

	//typedef std::vector<Site>		Sites;
	typedef std::vector<Point>		Points;
	typedef std::vector<Vertex_handle>	Vertex_handles;

	LineSegs	m_Lines;
	Points		m_Points;
	Delaunay	m_Delaunay;

	void AddPoint(double x, double y)
	{
		m_Points.push_back(Point(x, y));
	}
	void Compute()
	{
		m_Delaunay.insert(m_Points.begin(), m_Points.end());
		Delaunay::Finite_edges_iterator eit = m_Delaunay.finite_edges_begin();

		for (; eit != m_Delaunay.finite_edges_end(); ++eit)
		{
			CGAL::Object o = m_Delaunay.dual(eit);

			if (CGAL::object_cast<K::Segment_2>(&o))
			{
				const K::Segment_2* seg = CGAL::object_cast<K::Segment_2>(&o);
				Point p1(seg->source().hx(), seg->source().hy());
				Point p2(seg->target().hx(), seg->target().hy());
				m_Lines.push_back(
					LineSeg(
						Vector2(p1.hx(), p1.hy()), Vector2(p2.hx(), p2.hy())
						) 
					);
			}
		}
	}
	void Clear() {}

};



