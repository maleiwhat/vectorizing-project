#pragma once
#include <iostream>
#include "TriangulationBase.h"

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include "ImageSpline.h"
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
	ImageSpline	m_ImageSpline;


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

};



