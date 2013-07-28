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
Lines	GetLines(const Lines& cvp, double xOffset=0, double yOffset=0);
Lines	GetLines(const CgalLines& cvp, double xOffset=0, double yOffset=0);
Line	GetLine(const CvLine& cvp, double xOffset=0, double yOffset=0);
Line	GetLine(const Line& cvp, double xOffset=0, double yOffset=0);
Line	GetLine(const CgalLine& cvp, double xOffset=0, double yOffset=0);

CvLines	GetCvLines(const Lines& cvp, double xOffset=0, double yOffset=0);
CvLines	GetCvLines(const CgalLines& cvp, double xOffset=0, double yOffset=0);
CvLine	GetCvLine(const Line& cvp, double xOffset=0, double yOffset=0);
CvLine	GetCvLine(const CgalLine& cvp, double xOffset=0, double yOffset=0);

CgalLines	GetCgalLines(const CvLines& cvp, double xOffset=0, double yOffset=0);
CgalLines	GetCgalLines(const Lines& cvp, double xOffset=0, double yOffset=0);
CgalLine	GetCgalLine(const CvLine& cvp, double xOffset=0, double yOffset=0);
CgalLine	GetCgalLine(const Line& cvp, double xOffset=0, double yOffset=0);

Line	SmoothingLen5(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Line	SmoothingLen3(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines	SmoothingLen5(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines	SmoothingLen3(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);

Line	GetNormalsLen2(const Line& cvp);
Line	GetNormalsLen3(const Line& cvp);
Lines	GetNormalsLen2(const Lines& cvp);
Lines	GetNormalsLen3(const Lines& cvp);
Line	LineAdd(const Line& aLine, const Line& bLine);
Lines	LinesAdd(const Lines& aLine, const Lines& bLine);
Line	LineSub(const Line& aLine, const Line& bLine);
Lines	LinesSub(const Lines& aLine, const Lines& bLine);

Line	FixLineWidths(const Line& aLine, int range);
Lines	FixLineWidths(const Lines& aLine, int range);

Line	CleanOrphanedLineWidths(const Line& aLine, int num);
Lines	CleanOrphanedLineWidths(const Lines& aLine, int num);

Line	SmoothingHas0Len5(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Line	SmoothingHas0Len3(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines	SmoothingHas0Len5(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines	SmoothingHas0Len3(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);

Lines	SplitStraightLine(const Line& cvp, double angle);
Lines	SplitStraightLine(const Lines& cvp, double angle);

double_vector ComputeAngle(const Line& line);
Line    GetControlPoint(const Line& line, double angle);
