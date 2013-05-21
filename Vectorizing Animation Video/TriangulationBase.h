#pragma once
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>


struct Triangle
{
	Vector2 m_Points[3];
	Vector3 m_Colors[3];
};
typedef std::vector<Triangle> Triangles;

class TriangulationBase
{
public:
	virtual ~TriangulationBase() {}
	Triangles& GetTriangles()
	{
		return m_Triangles;
	}
protected:
	Triangles   m_Triangles;
};

