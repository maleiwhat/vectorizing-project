#pragma once

#include <vector>
#include <opencv2/core/core.hpp>
#include <deque>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
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

Lines	GetLines(const CvLines& cvp, double xOffset=0, double yOffset=0);
Lines	GetLines(const CgalLines& cvp, double xOffset=0, double yOffset=0);
Line	GetLine(const CvLine& cvp, double xOffset=0, double yOffset=0);
Line	GetLine(const CgalLine& cvp, double xOffset=0, double yOffset=0);

CvLines	GetCvLines(const Lines& cvp, double xOffset=0, double yOffset=0);
CvLines	GetCvLines(const CgalLines& cvp, double xOffset=0, double yOffset=0);
CvLine	GetCvLine(const Line& cvp, double xOffset=0, double yOffset=0);
CvLine	GetCvLine(const CgalLine& cvp, double xOffset=0, double yOffset=0);

CgalLines	GetCgalLines(const CvLines& cvp, double xOffset=0, double yOffset=0);
CgalLines	GetCgalLines(const Lines& cvp, double xOffset=0, double yOffset=0);
CgalLine	GetCgalLine(const CvLine& cvp, double xOffset=0, double yOffset=0);
CgalLine	GetCgalLine(const Line& cvp, double xOffset=0, double yOffset=0);

Line	SmoothingSize5(const Line& cvp, int repeat = 1);
Line	SmoothingSize3(const Line& cvp, int repeat = 1);
Lines	SmoothingSize5(const Lines& cvp, int repeat = 1);
Lines	SmoothingSize3(const Lines& cvp, int repeat = 1);
