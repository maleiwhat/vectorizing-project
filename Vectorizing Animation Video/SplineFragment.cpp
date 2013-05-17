#include "SplineFragment.h"



void SplineFragment::AddPoint(const Vector2& p)
{
	m_OriginPointSize++;
	m_sx.AddPoint(p[0]);
	m_sy.AddPoint(p[1]);
}

void SplineFragment::Clear()
{
	m_sx.Clear();
	m_sy.Clear();
}

Vector2 SplineFragment::GetVector2(double v)
{
	return Vector2(m_sx.GetPoint(v), m_sy.GetPoint(v));
}

int SplineFragment::PatchId1()
{
	return m_id_near1;
}

int SplineFragment::PatchId2()
{
	return m_id_near2;
}

void SplineFragment::CurveFitting(double nodes_scale /*= 0.2*/)
{
	m_sx.CurveFitting(nodes_scale);
	m_sy.CurveFitting(nodes_scale);
}
