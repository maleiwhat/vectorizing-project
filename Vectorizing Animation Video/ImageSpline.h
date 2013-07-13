#pragma once
#include "PatchSpline.h"
#include "SplineFragment.h"
#include "Patch.h"
#include "vavImage.h"

// �N�h�� PatchSpline ��X���@�� ImageSpline
// �̭��s�ۦh�� SplineFragment

class ImageSpline
{
public:
	ImageSpline(void);
	~ImageSpline(void);
	void AddPatchSpline(const PatchLine& ps);
	void ComputeToLineFragments();
	void SmoothingFragments();
	bool CheckOnSide(int x, int y);
	void SetSize(int w, int h);
	void RemoveShadingLine(vavImage& img);
	CvPatchs    m_CvPatchs;
	SplineFragments& GetSplineFragments();
	Lines       m_Controls;
	LineFragments   m_LineFragments;
	LineFragments   m_ControlPoints;
	PatchSplines    m_PatchSplines;
	PatchSplines2d  m_PatchSplinesInter;
	int     m_w, m_h;
private:
	PatchLines  m_PatchLines;
	SplineFragments m_SplineFragments;
};

