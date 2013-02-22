#pragma once
#include <vector>
#include "Edge.h"

struct LineFragment
{
	// 鄰近哪一塊patch -1 表示沒有
	int m_id_near;
	// 屬於哪個patch
	int m_id;
	Line m_Points;
	void AddCvPoint(cv::Point& p);
};
typedef std::vector<LineFragment> LineFragments;
