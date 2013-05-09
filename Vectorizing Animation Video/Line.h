#pragma once

#include <vector>
#include <opencv2/core/core.hpp>
#include <deque>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include "math/Vector2.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;
typedef std::vector<Point> Points;
typedef std::vector<Points> Points2d;

typedef std::vector<Vector2> Line;
typedef std::vector<Line> Lines;
typedef std::vector<cv::Point> CvPoints;
typedef std::vector<CvPoints> CvPoints2d;
struct LineSeg
{
	
	Vector2 beg, end;
	LineSeg(){}
	LineSeg(Vector2& b, Vector2& e):beg(b), end(e){}
};
typedef std::deque<LineSeg> LineSegs;
