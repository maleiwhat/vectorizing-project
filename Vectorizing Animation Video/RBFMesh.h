#pragma once
#include "BasicMesh.h"
#include "TriangulationBase.h"

class TriangulationCgal_SeedPoint;
class RBFMesh : public BasicMesh
{
public:
	RBFMesh(void);
	~RBFMesh(void);
	void ReadFromSeedpoint(TriangulationCgal_SeedPoint* data, Vector3s& colors);
	Vector3 GetColorVector3(double x, double y);
	Vector3 GetColorVector3(double x1, double y1, double x2, double y2);
	void lightblurC2();
	void blurC2();

	void MakeColor1();
	ColorTriangles	m_Trangles;
};

