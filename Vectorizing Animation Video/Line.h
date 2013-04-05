#pragma once

#include <vector>
#include "math/Vector2.h"
#include <opencv2/core/core.hpp>

typedef Vector2s Line;
typedef std::vector<Line> Lines;
typedef std::vector<cv::Point> CvPoints;
typedef std::vector<CvPoints> CvPoints2d;
struct LineSeg
{
	
	Vector2 beg, end;
	LineSeg(){}
	LineSeg(Vector2& b, Vector2& e):beg(b), end(e){}
};
typedef std::vector<LineSeg> LineSegs;
