#pragma once
#include "algSpline2d.h"
#include <opencv2\core\core.hpp>
#include "math/Vector3.h"

double GetLight(const Vector3& v);
bool LightCompareVector3(const Vector3& v1, const Vector3& v2);

class ColorConstraintAverage
{
public:
	ColorConstraintAverage();
	void AddPoint(double x, double y, const cv::Vec3b& p);
	void AddPoint(double x, double y, const Vector3& p);
	void Clear();
	Vector3 GetColorVector3();
	cv::Vec3b GetColorCvPoint();
private:
	void ComputeAverage();
	Vector3s m_Colors;
	Vector3  m_Average;
	bool m_NeedComputeAverage;
};

