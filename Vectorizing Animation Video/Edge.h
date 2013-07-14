#pragma once
#include <opencv2\core\core.hpp>
#include "Line.h"

struct CEdge
{
	CEdge(int _index) {index = _index; }
	~CEdge(void) {}
	Line GetLine() const;
	// Domains assigned during link();
	int index;    // Start from 0
	int pointNum;
	cv::Point start, end;
	CvLine pnts;
};
typedef std::vector<CEdge> CEdges;

Lines GetLines(const CEdges& edges);

