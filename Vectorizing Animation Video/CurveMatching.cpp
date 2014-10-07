#include "CurveMatching.h"

double ColorDistance(const Vector3s& c1, const Vector3s& c2)
{
	if(c1.size() < 3 || c2.size() < 3)
	{
		return 99999999999;
	}
	double sum = 0;
	for(int i = 0; i < c1.size(); ++i)
	{
		double percent = (double)i / (c1.size() - 1);
		int j = percent * (c2.size() - 1);
		sum += c1[i].distance(c2[j]);
	}
	return sum / c1.size();
}

double LineDistance(const Line& l1, const Line& l2)
{
	if(l1.size() < 3 || l2.size() < 3)
	{
		return 99999999999;
	}
	double sum = 0;
	for(int i = 0; i < l1.size(); ++i)
	{
		double percent = (double)i / (l1.size() - 1);
		int j = percent * (l2.size() - 1);
		sum += l1[i].distance(l2[j]);
	}
	return sum / l1.size();
}

bool IsNeedReCurve(Line& line)
{
	if(line.size() > 1)
	{
		double dis = (line.front().x - line.front().x) + (line.front().y - line.front().y);
		if(dis < 0)
		{
			return true;
		}
	}
	return false;
}

CurveMatching ComputeMatching(FrameInfo& fi1, FrameInfo& fi2, cv::Vec2f move)
{
	CurveMatching res;
	Vector3s2d color11 = fi1.GetLine1Color();
	Vector3s2d color21 = fi2.GetLine1Color();
	Color2Side color12 = fi1.GetLine2Color();
	Color2Side color22 = fi2.GetLine2Color();
	for(int i = 0; i < fi1.curves1.size(); ++i)
	{
		if(IsNeedReCurve(fi1.curves1[i]))
		{
			std::reverse(fi1.curves1[i].begin(), fi1.curves1[i].end());
			std::reverse(color11[i].begin(), color11[i].end());
		}
	}
	for(int i = 0; i < fi2.curves1.size(); ++i)
	{
		if(IsNeedReCurve(fi2.curves1[i]))
		{
			std::reverse(fi2.curves1[i].begin(), fi2.curves1[i].end());
			std::reverse(color21[i].begin(), color21[i].end());
		}
		for(int j = 0; j < fi2.curves1[i].size(); ++j)
		{
			fi2.curves1[i][j].x -= move[0];
			fi2.curves1[i][j].y -= move[1];
		}
	}
	for(int i = 0; i < fi1.curves1.size(); ++i)
	{
		int minid = -1;
		double v1 = 10000;
		for(int j = 0; j < fi2.curves1.size(); ++j)
		{
			double sdiff = LineDistance(fi1.curves1[i], fi2.curves1[j]);
			double cdiff = ColorDistance(color11[i], color21[j]);
			double diff = sdiff;
			if(diff < v1)
			{
				minid = j;
				v1 = diff;
			}
		}
		if(minid != -1)
		{
			res.curve1.push_back(i);
			res.curve2.push_back(minid);
		}
	}
	for(int i = 0; i < fi2.curves1.size(); ++i)
	{
		for(int j = 0; j < fi2.curves1[i].size(); ++j)
		{
			fi2.curves1[i][j].x += move[0];
			fi2.curves1[i][j].y += move[1];
		}
	}
	return res;
}

