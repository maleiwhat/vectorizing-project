#pragma once
#include <vector>
#include "algSpline.h"
#include "math\Vector2.h"

class SplineFragment
{
private:
	// 鄰近的第一塊patch
	int m_id_near1;
	// 鄰近的第二塊patch
	int m_id_near2;
	// 原來這條線由幾個點所構成
	int m_OriginPointSize;
	algSpline m_sx, m_sy;
public:
	SplineFragment():m_OriginPointSize(0)
	{}
	void SetPatchId(int i1, int i2);
	int  PatchId1();
	int  PatchId2();
	void AddPoint(const Vector2& p);
	void Clear();
	Vector2 GetVector2(double v);
	void CurveFitting(double nodes_scale = 0.2);
};
typedef std::vector<SplineFragment> SplineFragments;
