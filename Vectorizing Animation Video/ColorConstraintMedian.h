#pragma once
#include "algSpline2d.h"
#include <opencv2\core\core.hpp>
#include "math/Vector3.h"
#include "shared_ptr.h"

float GetLight(const Vector3& v);
bool LightCompareVector3(const Vector3& v1, const Vector3& v2);

class ColorConstraintMedian
{
public:
	ColorConstraintMedian();
	void AddPoint(double x, double y, const cv::Vec3b& p);
	void AddPoint(double x, double y, const Vector3& p);
	void Clear();
	Vector3 GetColorVector3(double x, double y);
	cv::Vec3b GetColorCvPoint(double x, double y);
private:
	void ComputeMedian();
	Vector3s m_Colors;
	Vector3  m_Median;
	bool m_NeedComputeMedian;
};
SHARE_PTR(ColorConstraintMedian);

typedef ColorConstraintMedian ColorConstraint;
typedef ColorConstraintMedian_sptr ColorConstraint_sptr;
typedef std::vector<ColorConstraint_sptr> ColorConstraint_sptrs;
