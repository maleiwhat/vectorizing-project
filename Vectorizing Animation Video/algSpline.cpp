#include "algSpline.h"
#include <cassert>

using namespace alglib;

algSpline::algSpline() : m_rho( 0.3 ), m_NeedBuildSpline( false )
{
}


algSpline::~algSpline( void )
{
}

void algSpline::AddPoint( double v )
{
	m_NeedBuildSpline = true;
	m_vV.push_back( v );
}

double algSpline::GetPoint( double t )
{
	assert( m_vV.size() > 2 );
	CheckBuildSpline();
	return spline1dcalc( m_Interpolant, t );
}

doubles algSpline::GetSideline()
{
	assert( m_vV.size() > 2 );
	CheckBuildSpline();
	double step = 1.0 / ( m_vV.size() - 1 );
	double start = 0.0;
	doubles res;
	double firstpoint, lastpoint;
	firstpoint = spline1dcalc( m_Interpolant, 0.0 );
	lastpoint = firstpoint;

	// 找到下一個鄰近的 pixel 直到起始點
	for ( ;; )
	{
		double nowpoint;
		nowpoint = round( spline1dcalc( m_Interpolant, start + step ) );

		if ( abs( nowpoint - lastpoint ) >= 2 )
		{
			step *= 0.5;
			continue;
		}

		if ( lastpoint == nowpoint )
		{
			step *= 3;
			continue;
		}
		else
		{
			res.push_back( nowpoint );
			lastpoint = nowpoint;
			start += step;

			if ( nowpoint == firstpoint || start >= 0 )
			{
				break;
			}
		}
	}

	return res;
}

void algSpline::CheckBuildSpline()
{
	if (m_NeedBuildSpline)
	{
		m_NeedBuildSpline = false;
		m_aV.setcontent( m_vV.size(), &m_vV[0] );
		doubles scale( m_vV.size() );

		for ( int i = 0; i < m_vV.size(); ++i )
		{
			scale[i] = i * 1.0 / ( m_vV.size() - 1 );
		}
		m_aScale.setcontent( scale.size(), &scale[0] );
		spline1dbuildcubic(m_aScale, m_aV, m_Interpolant);	
	}
}

void algSpline::Clear()
{
	m_NeedBuildSpline = true;
	m_vV.clear();
}

void algSpline::CurveFitting(double nodes_scale)
{
	m_NeedBuildSpline = false;
	m_aV.setcontent( m_vV.size(), &m_vV[0] );
	doubles scale( m_vV.size() );

	for ( int i = 0; i < m_vV.size(); ++i )
	{
		scale[i] = i * 1.0 / ( m_vV.size() - 1 );
	}

	m_aScale.setcontent( scale.size(), &scale[0] );
	int nodes = m_vV.size() * nodes_scale;

	if ( nodes < 5 ) { nodes = 5; }

	spline1dfitpenalized( m_aScale, m_aV, nodes, m_rho, m_SplineFunctionInfo, m_Interpolant, m_FitReport );
}

