#include "ColorConstraintAverage.h"

void ColorConstraintAverage::AddPoint(double x, double y, const cv::Vec3b& p)
{
	m_NeedComputeAverage = true;
	m_Colors.push_back(Vector3(p[0], p[1], p[2]));
}

void ColorConstraintAverage::AddPoint(double x, double y, const Vector3& p)
{
	m_NeedComputeAverage = true;
	m_Colors.push_back(p);
}

void ColorConstraintAverage::Clear()
{
	m_NeedComputeAverage = true;
	m_Colors.clear();
}

Vector3 ColorConstraintAverage::GetColorVector3()
{
	if (m_NeedComputeAverage)
	{
		ComputeAverage();
	}

	return m_Average;
}

cv::Vec3b ColorConstraintAverage::GetColorCvPoint()
{
	if (m_NeedComputeAverage)
	{
		ComputeAverage();
	}

	cv::Vec3b res;
	res[0] = m_Average[0];
	res[1] = m_Average[1];
	res[2] = m_Average[2];
	return res;
}

ColorConstraintAverage::ColorConstraintAverage(): m_NeedComputeAverage(true),
	m_Average(0, 0, 0)
{
}

void ColorConstraintAverage::ComputeAverage()
{
	m_NeedComputeAverage = false;

	if (!m_Colors.empty())
	{
		Vector3 sum;
		for (int i=0;i<m_Colors.size();i++)
		{
			sum += m_Colors[i];
		}
		m_Average = sum / m_Colors.size();
	}

	//printf("color r: %f g: %f b: %f\n", m_Average[0], m_Average[1], m_Average[2]);
}
