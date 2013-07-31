#pragma once
#pragma warning( push, 0 )
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#pragma warning( pop ) 

#include <vector>
#include <opencv2/core/core.hpp>
#include <deque>
#include "math/Vector2.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel CgalInexactKernel;
typedef CgalInexactKernel::Point_2 CgalPoint;
typedef std::vector<CgalPoint> CgalLine;
typedef std::vector<CgalLine> CgalLines;

typedef std::vector<Vector2> Line;
typedef std::vector<Line> Lines;
typedef std::vector<cv::Point> CvLine;
typedef std::vector<CvLine> CvLines;
struct LineSeg
{
	Vector2 beg, end;
	LineSeg() {}
	LineSeg(Vector2& b, Vector2& e): beg(b), end(e) {}
};
typedef std::deque<LineSeg> LineSegs;

struct Endpoint
{
	Vector2 pos;
	int	idx1, idx2;
	double	angle;
	Endpoint() {}
	Endpoint(const Vector2& p, int i1, int i2, double a)
		: pos(p), idx1(i1), idx2(i2), angle(a) {}
	bool operator==(const Endpoint& rhs)
	{
		return idx1 == rhs.idx1 && idx2 == rhs.idx2;
	}
	bool operator!=(const Endpoint& rhs)
	{
		return idx1 != rhs.idx1 || idx2 != rhs.idx2;
	}
};
typedef std::vector<Endpoint> Endpoints;