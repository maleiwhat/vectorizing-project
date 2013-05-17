#include "SplineShape.h"
#include <cassert>

using namespace alglib;

SplineShape::SplineShape() : m_rho(1.5), m_NeedBuildSpline(false)
{
}


SplineShape::~SplineShape(void)
{
}

void SplineShape::AddPoint(double x, double y)
{
	m_NeedBuildSpline = true;
	m_vX.push_back(x);
	m_vY.push_back(y);
}

Vector2 SplineShape::GetPoint(double t)
{
	assert(m_vX.size() > 2);
	CheckBuildSpline();
	Vector2 point;
	point.x = spline1dcalc(m_InterpolantX, t);
	point.y = spline1dcalc(m_InterpolantY, t);
	return point;
}

Vector2s SplineShape::GetSideline()
{
	assert(m_vX.size() > 2);
	CheckBuildSpline();
	double step = 1.0 / (m_vX.size() - 1);
	double start = 0.0;
	Vector2s res;
	Vector2 firstpoint, lastpoint;
	firstpoint.x = spline1dcalc(m_InterpolantX, 0.0);
	firstpoint.y = spline1dcalc(m_InterpolantY, 0.0);
	lastpoint = firstpoint;

	// 找到下一個鄰近的 pixel 直到起始點
	for (;;)
	{
		Vector2 nowpoint;
		nowpoint.x = round(spline1dcalc(m_InterpolantX, start + step));

		if (abs(nowpoint.x - lastpoint.x) >= 2)
		{
			step *= 0.5;
			continue;
		}

		nowpoint.y = round(spline1dcalc(m_InterpolantY, start + step));

		if (abs(nowpoint.y - lastpoint.y) >= 2)
		{
			step *= 0.5;
			continue;
		}

		if (lastpoint == nowpoint)
		{
			step *= 3;
			continue;
		}
		else
		{
			res.push_back(nowpoint);
			lastpoint = nowpoint;
			start += step;

			if (nowpoint == firstpoint || start >= 0)
			{
				break;
			}
		}
	}

	return res;
}

void SplineShape::CheckBuildSpline()
{
	if (m_NeedBuildSpline)
	{
		m_NeedBuildSpline = false;
		m_aX.setcontent(m_vX.size(), &m_vX[0]);
		m_aY.setcontent(m_vY.size(), &m_vY[0]);
		doubles scale(m_vX.size());

		for (int i = 0; i < m_vX.size(); ++i)
		{
			scale[i] = i * 1.0 / (m_vX.size() - 1);
		}

		m_aScale.setcontent(scale.size(), &scale[0]);
		int nodes = m_vX.size() / 5;

		if (nodes < 5) { nodes = 5; }

		spline1dbuildcubic(m_aScale, m_aX, m_InterpolantX);
		spline1dbuildcubic(m_aScale, m_aY, m_InterpolantY);
//      spline1dfitpenalized( m_aScale, m_aX, nodes, m_rho, m_SplineFunctionInfo, m_InterpolantX, m_FitReport );
//      spline1dfitpenalized( m_aScale, m_aY, nodes, m_rho, m_SplineFunctionInfo, m_InterpolantY, m_FitReport );
	}
}

void SplineShape::Clear()
{
	m_NeedBuildSpline = true;
	m_vX.clear();
	m_vY.clear();
}

void SplineShape::CurveFitting(double nodes_scale)
{
	m_NeedBuildSpline = false;
	m_aX.setcontent(m_vX.size(), &m_vX[0]);
	m_aY.setcontent(m_vY.size(), &m_vY[0]);
	doubles scale(m_vX.size());

	for (int i = 0; i < m_vX.size(); ++i)
	{
		scale[i] = i * 1.0 / (m_vX.size() - 1);
	}

	m_aScale.setcontent(scale.size(), &scale[0]);
	int nodes = m_vX.size() * nodes_scale;

	if (nodes < 5) { nodes = 5; }

	spline1dfitpenalized(m_aScale, m_aX, nodes, m_rho, m_SplineFunctionInfo,
	                     m_InterpolantX, m_FitReport);
	spline1dfitpenalized(m_aScale, m_aY, nodes, m_rho, m_SplineFunctionInfo,
	                     m_InterpolantY, m_FitReport);
}
