#pragma once
#include "PatchSpline.h"
#include "SplineFragment.h"

// 將多個 PatchSpline 整合成一個 ImageSpline
// 裡面存著多個 SplineFragment

class ImageSpline
{
public:
	ImageSpline(void);
	~ImageSpline(void);
	void AddPatchSpline(const PatchLine& ps);
	void ComputeToLineFragments();
	void ComputeToSplineFragments();

	SplineFragments& GetSplineFragments();
	LineFragments	m_LineFragments;
	LineFragments	m_ControlPoints;
	PatchSplines	m_PatchSplines;
private:
	PatchLines	m_PatchLines;
	SplineFragments m_SplineFragments;
	
};

