#pragma once
#include <vector>
#include "algSpline.h"
#include "math\Vector2.h"

class SplineFragment
{
private:
	// �F�񪺲Ĥ@��patch
	int m_id_near1;
	// �F�񪺲ĤG��patch
	int m_id_near2;
	// ��ӳo���u�ѴX���I�Һc��
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
