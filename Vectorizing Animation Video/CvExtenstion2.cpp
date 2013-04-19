
#define _USE_MATH_DEFINES
#include <cmath>
#include "CvExtenstion2.h"

double_vector Curvature(const Line& line)
{
	double_vector ans(line.size(), 0);

	for (int i = 1; i < line.size(); i++)
	{
		Vector2 vec = line[i] - line[i-1];
		ans[i] = atan2f(vec.x, vec.y) / M_PI * 180;
	}
}

double_vector Accumulation(const double_vector& line)
{
	double_vector ans = line;

	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = ans[i] + ans[i-1];
	}
}

double_vector AbsAccumulation(const double_vector& line)
{
	double_vector ans = line;
	ans[0] = abs(ans[0]);
	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = abs(ans[i]) + ans[i-1];
	}
}

