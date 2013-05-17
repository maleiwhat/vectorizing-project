#include "LineFragment.h"


void LineFragment::AddCvPoint(cv::Point& p)
{
	m_Points.push_back(Vector2(p.x, p.y));
}
