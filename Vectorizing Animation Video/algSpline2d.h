#pragma once
#include <vector>
#include <interpolation.h>

typedef std::vector<double> doubles;
class algSpline2d
{
public:
	algSpline2d(void);
	~algSpline2d(void);
	void AddPoint(double x, double y, double v);
	void Clear();
	// input 0~1 get position
	double GetPoint(double x, double y);
private:
	void CheckBuildSpline();

	doubles m_vV;
	alglib::ae_int_t		m_SplineFunctionInfo;
	alglib::rbfmodel		m_model;
	alglib::rbfreport		m_rep;
	alglib::real_2d_array		m_data;
	double	m_rho;
	bool	m_NeedBuildSpline;
};
