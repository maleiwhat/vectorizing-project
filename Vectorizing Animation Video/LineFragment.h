#pragma once
#include <vector>
#include "Edge.h"

struct LineFragment
{
	// �F����@��patch -1 ��ܨS��
	int m_id_near;
	// �ݩ����patch
	int m_id;
	Line m_Points;
	void AddCvPoint(cv::Point& p);
};
typedef std::vector<LineFragment> LineFragments;
