#include "algSpline2d.h"
#include <cassert>

using namespace alglib;

algSpline2d::algSpline2d() : m_rho(0.3), m_NeedBuildSpline(false)
{
	rbfcreate(2, 1, m_model);
}


algSpline2d::~algSpline2d(void)
{
}

void algSpline2d::AddPoint(double x, double y, double v)
{
	m_NeedBuildSpline = true;
	m_vV.push_back(x);
	m_vV.push_back(y);
	m_vV.push_back(v);
}

double algSpline2d::GetPoint(double x, double y)
{
	assert(m_vV.size() > 2);
	CheckBuildSpline();
	return rbfcalc2(m_model, x, y);
}

void algSpline2d::CheckBuildSpline()
{
	if (m_NeedBuildSpline)
	{
		m_NeedBuildSpline = false;
		m_data.setcontent(m_vV.size() / 3, 3, &m_vV[0]);
		rbfsetpoints(m_model, m_data);
		int new_size = m_vV.size() / 30;

		if (new_size < 10) { new_size = 10; }

		if (new_size > 200) { new_size = 200; }

		rbfsetalgomultilayer(m_model, 5.0, new_size, 1.0e-2);
		rbfbuildmodel(m_model, m_rep);
	}
}

void algSpline2d::Clear()
{
	m_NeedBuildSpline = true;
	m_vV.clear();
}
