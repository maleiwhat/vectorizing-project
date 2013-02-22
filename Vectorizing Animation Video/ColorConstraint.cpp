#include "ColorConstraint.h"

void ColorConstraintRBF::AddPoint( double x, double y, const cv::Vec3b& p )
{
	m_sr.AddPoint(x, y, p[0]);
	m_sg.AddPoint(x, y, p[1]);
	m_sb.AddPoint(x, y, p[2]);
}

void ColorConstraintRBF::AddPoint( double x, double y, const Vector3& p )
{
	m_sr.AddPoint(x, y, p.x);
	m_sg.AddPoint(x, y, p.y);
	m_sb.AddPoint(x, y, p.z);
}

Vector3 ColorConstraintRBF::GetColorVector3( double x, double y )
{
	Vector3 res;
	res.x = m_sr.GetPoint(x, y);
	res.y = m_sg.GetPoint(x, y);
	res.z = m_sb.GetPoint(x, y);
	return res;
}

cv::Vec3b ColorConstraintRBF::GetColorCvPoint( double x, double y )
{
	cv::Vec3b res;
	res[0] = m_sr.GetPoint(x, y);
	res[1] = m_sg.GetPoint(x, y);
	res[2] = m_sb.GetPoint(x, y);
	return res;
}

void ColorConstraintRBF::Clear()
{
	m_sr.Clear();
	m_sg.Clear();
	m_sb.Clear();
}
