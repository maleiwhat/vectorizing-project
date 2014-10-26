#pragma once
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

struct Triangle
{
	Vector2 pts[3];
};
typedef std::vector<Triangle> Triangles;
typedef std::vector<Triangles> Triangles2d;

struct ColorTriangle
{
	Vector2 pts[3];
	Vector3 color[3];
};
typedef std::vector<ColorTriangle> ColorTriangles;

class TriangulationBase
{
public:
	virtual ~TriangulationBase() {}
	ColorTriangles& GetTriangles()
	{
		return m_Triangles;
	}
protected:
	ColorTriangles   m_Triangles;
};

