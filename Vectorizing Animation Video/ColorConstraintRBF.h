#pragma once
#include "algSpline2d.h"
#include <opencv2\core\core.hpp>
#include "math/Vector3.h"
#include "shared_ptr.h"

class ColorConstraintRBF
{
public:
	void AddPoint(double x, double y, const cv::Vec3b& p);
	void AddPoint(double x, double y, const Vector3& p);
	void Clear();
	Vector3 GetColorVector3(double x, double y);
	cv::Vec3b GetColorCvPoint(double x, double y);
	algSpline2d m_sr, m_sg, m_sb;
};
SHARE_PTR(ColorConstraintRBF);
