#pragma once
#include "algSpline2d.h"
#include <opencv2\core\core.hpp>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

double GetLight(const Vector3& v);
bool LightCompareVector3(const Vector3& v1, const Vector3& v2);

class ColorConstraintMedian
{
public:
	ColorConstraintMedian();
	void AddPoint(double x, double y, const cv::Vec3b& p);
	void AddPoint(double x, double y, const Vector3& p);
	void Clear();
	Vector3 GetColorVector3();
	Vector3 GetColorVector3(double x, double y);
	cv::Vec3b GetColorCvPoint();
private:
	void ComputeMedian();
	Vector3s m_Colors;
	Vector3s m_ColorsOri;
	Vector2s m_Pos;
	Vector3  m_Median;
	bool m_NeedComputeMedian;
};
typedef std::vector<ColorConstraintMedian> ColorConstraintMedians;
