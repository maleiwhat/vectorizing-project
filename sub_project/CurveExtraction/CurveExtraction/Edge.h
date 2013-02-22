#pragma once
#include <opencv2\core\core.hpp>
#include <vector>
#include "math/Vector2.h"

typedef Vector2s Line;
typedef std::vector<Line> Lines;
typedef std::vector<cv::Point> CvPoints;
typedef std::vector<CvPoints> CvPoints2d;

struct CEdge
{
	CEdge( int _index ) {index = _index; }
	~CEdge( void ) {}
	Line GetLine() const;
	// Domains assigned during link();
	int index;    // Start from 0
	int pointNum;
	cv::Point start, end;
	CvPoints pnts;
};
typedef std::vector<CEdge> CEdges;

Lines GetLines(const CEdges& edges);
Lines GetLine(CvPoints2d& cvline);
