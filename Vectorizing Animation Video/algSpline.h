#pragma once
#include <vector>
#include <interpolation.h>

typedef std::vector<double> doubles;
class algSpline
{
public:
	algSpline(void);
	~algSpline(void);
	void AddPoint(double v);
	void Clear();
	// input 0~1 get position
	double GetPoint(double t);
	// get pixel space positions
	doubles GetSideline();
	void CurveFitting(double nodes_scale = 0.2);
private:
	void CheckBuildSpline();

	doubles m_vV;
	alglib::spline1dinterpolant	m_Interpolant;
	alglib::spline1dfitreport	m_FitReport;
	alglib::real_1d_array		m_aV;
	alglib::real_1d_array		m_aScale;
	alglib::ae_int_t		m_SplineFunctionInfo;
	double	m_rho;
	bool	m_NeedBuildSpline;
};
