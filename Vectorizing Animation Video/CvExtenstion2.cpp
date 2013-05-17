#include "stdcv.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include "CvExtenstion2.h"

double_vector ComputeAngle(const Line& line)
{
	double_vector ans(line.size(), 0);

	for (int i = 1; i < line.size(); i++)
	{
		Vector2 vec = line[i] - line[i - 1];
		ans[i] = atan2f(vec.x, vec.y) / M_PI * 180;
	}

	ans[0] = ans[1];
	return ans;
}

double_vector Curvature(const double_vector& angles)
{
	double_vector ans = angles;
	ans[0] = 0;

	for (int i = 1; i < ans.size(); i++)
	{
		ans[i] = angles[i] - angles[i - 1];
	}

	return ans;
}
double_vector Accumulation(const double_vector& line)
{
	double_vector ans = line;

	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = ans[i] + ans[i - 1];
	}

	return ans;
}

double_vector AbsAccumulation(const double_vector& line)
{
	double_vector ans = line;
	ans[0] = abs(ans[0]);

	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = abs(ans[i]) + ans[i - 1];
	}

	return ans;
}

Line GetControlPoint(const Line& line, double angle)
{
	Line ans;
	double_vector angles = Accumulation(Curvature(ComputeAngle(line)));
	double threshold = 0;
	assert(angles.size() == line.size());
	ans.push_back(line.front());

	for (int i = 1; i < angles.size() - 1; ++i)
	{
		if (abs(angles[i] - threshold) > angle)
		{
			ans.push_back(line[i]);
			threshold = angles[i];
		}
	}

	ans.push_back(line.back());
	return ans;
}

