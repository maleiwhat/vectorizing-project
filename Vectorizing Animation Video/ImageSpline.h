#pragma once
#include "PatchSpline.h"
#include "SplineFragment.h"

// �N�h�� PatchSpline ��X���@�� ImageSpline
// �̭��s�ۦh�� SplineFragment

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

