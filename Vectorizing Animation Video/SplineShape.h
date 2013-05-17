#pragma once
#include <vector>
#include <auto_link_alglib.hpp>
#include <interpolation.h>
#include "math/Vector2.h"

typedef std::vector<double> doubles;
class SplineShape
{
public:
	SplineShape(void);
	~SplineShape(void);
	void AddPoint(double x, double y);
	void Clear();
	// input 0~1 get position
	Vector2 GetPoint(double t);
	// get pixel space positions
	Vector2s GetSideline();
	void CurveFitting(double nodes_scale = 0.2);
private:
	void CheckBuildSpline();

	doubles m_vX;
	doubles m_vY;
	alglib::spline1dinterpolant m_InterpolantX;
	alglib::spline1dinterpolant m_InterpolantY;
	alglib::spline1dfitreport   m_FitReport;
	alglib::real_1d_array       m_aX;
	alglib::real_1d_array       m_aY;
	alglib::real_1d_array       m_aScale;
	alglib::ae_int_t        m_SplineFunctionInfo;
	double  m_rho;
	bool    m_NeedBuildSpline;
};

