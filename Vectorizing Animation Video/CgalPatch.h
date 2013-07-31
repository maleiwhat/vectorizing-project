#pragma once
#pragma warning( push, 0 )
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#pragma warning( pop ) 
#include <vector>
#include "ImageSpline.h"

class CgalPatch
{
public:
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef K::Point_2 Point;
	typedef std::vector<Point> Points;
	typedef std::vector<Points> Points2d;
	CgalPatch(void);
	~CgalPatch(void);
	void    SetData(ImageSpline& is);
	Points&     Outer() {return m_Outer;}
	Points2d&   Inter() {return m_Inter;}
	bool CheckInside(double x, double y);
	void Clear();
	void insert_polygon(ImageSpline& is, int idx);
	void insert_polygonInter(ImageSpline& is, int idx);
	void insert_polygonInter2(ImageSpline& is, PatchSpline& ps);
private:
	Points      m_Outer;
	Points2d    m_Inter;
};
typedef std::vector<CgalPatch> CgalPatchs;

CgalPatchs MakePatchs(ImageSpline& is);

