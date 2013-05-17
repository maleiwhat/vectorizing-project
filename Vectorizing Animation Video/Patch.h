#pragma once
#include <math/Vector2.h>
#include <math/Vector3.h>
#include <opencv2/core/core.hpp>
#include "Edge.h"
#include "ColorConstraintMedian.h"

typedef std::vector<cv::Vec3b> Vec3bs;
typedef std::vector<Vec3bs> Vec3bs2d;

class CvPatch
{
public:
	void    AddOuterPoint(double x, double y);
	CvPoints&   Outer();
	CvPoints&   Outer2();
	CvPoints2d& Inter();
	CvPoints2d& Inter2();
	Vec3bs&     OuterColor();
	Vec3bs2d&   InterColor();
	void    Clear();
	void    SetImage(cv::Mat& image);
	ColorConstraint_sptr GetColorConstraint();
	bool    Inside(double x, double y);
private:
	CvPoints    m_Outer;
	CvPoints    m_Outer2;
	Vec3bs      m_OuterColor;
	CvPoints2d  m_Inter;
	CvPoints2d  m_Inter2;
	Vec3bs2d    m_InterColor;
	cv::Mat*    m_refImage;
};
typedef std::vector<CvPatch> CvPatchs;

class Patch
{
public:
	void    AddOuterPoint(double x, double y);
	Vector2s&   Outer();
	Vector2s2d& Inter();
	Vector3s&   OuterColor();
	Vector3s2d& InterColor();
	void    Clear();
	void    SplinePoints(double scale);
	void    Inflate(double len);
	void    Deflate(double len);
	ColorConstraint_sptr GetColorConstraint();
	void    SmoothPatch();
private:
	Vector2s    m_Outer;
	Vector3s    m_OuterColor;
	Vector2s2d  m_Inter;
	Vector3s2d  m_InterColor;
};
typedef std::vector<Patch> Patchs;

Patch   ToPatch(CvPatch& cvp);
CvPatch ToCvPatch(Patch& cvp);
