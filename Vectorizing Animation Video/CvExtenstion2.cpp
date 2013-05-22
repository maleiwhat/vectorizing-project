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

ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask,
                                 const cv::Mat& img)
{
	ColorConstraint_sptrs ans;

	for (int i = 0; i < regions; ++i)
	{
		ans.push_back(ColorConstraint_sptr(new ColorConstraint));
	}

	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			const cv::Vec3b& id_color = mask.at<cv::Vec3b>(i, j);
			int idx = id_color[0] + id_color[1] * 255 + id_color[2] * 255 * 255;
			const cv::Vec3b& color = img.at<cv::Vec3b>(i, j);
			ans[idx%ans.size()]->AddPoint(j, i, Vector3(color[0], color[1], color[2]));
		}
	}
	return ans;
}

