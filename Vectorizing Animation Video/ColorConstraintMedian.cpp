#include "ColorConstraintMedian.h"

double GetLight(const Vector3& v)
{
	return v[0] * 0.299 + v[1] * 0.587 + v[2] * 0.114;
}

bool LightCompareVector3(const Vector3& v1, const Vector3& v2)
{
	return GetLight(v1) < GetLight(v2);
}

void ColorConstraintMedian::AddPoint(double x, double y, const cv::Vec3b& p)
{
	m_NeedComputeMedian = true;
	m_Colors.push_back(Vector3(p[0], p[1], p[2]));
}

void ColorConstraintMedian::AddPoint(double x, double y, const Vector3& p)
{
	m_NeedComputeMedian = true;
	m_Colors.push_back(p);
}

void ColorConstraintMedian::Clear()
{
	m_NeedComputeMedian = true;
	m_Colors.clear();
}

Vector3 ColorConstraintMedian::GetColorVector3(double x, double y)
{
	if (m_NeedComputeMedian)
	{
		ComputeMedian();
	}

	return m_Median;
}

cv::Vec3b ColorConstraintMedian::GetColorCvPoint(double x, double y)
{
	if (m_NeedComputeMedian)
	{
		ComputeMedian();
	}

	cv::Vec3b res;
	res[0] = m_Median[0];
	res[1] = m_Median[1];
	res[2] = m_Median[2];
	return res;
}

ColorConstraintMedian::ColorConstraintMedian(): m_NeedComputeMedian(true),
	m_Median(0, 0, 0)
{
}

void ColorConstraintMedian::ComputeMedian()
{
	m_NeedComputeMedian = false;

	if (!m_Colors.empty())
	{
		std::sort(m_Colors.begin(), m_Colors.end(), LightCompareVector3);
		m_Median = m_Colors[m_Colors.size() / 2];
	}

	printf("color r: %f g: %f b: %f\n", m_Median[0], m_Median[1], m_Median[2]);
}
