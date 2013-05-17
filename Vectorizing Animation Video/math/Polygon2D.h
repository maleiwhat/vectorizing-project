#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "BasicMath.h"
#include <vector>


class Polygon2D
{
public:
	Polygon2D()
		: m_angle(0) {}
	Vec2s& Points()
	{
		m_needBuildEdges = true;
		return m_points;
	}
	const Vec2s& const_Points() const
	{
		return m_points;
	}
	void AddPoint(double x, double y);
	void AddPoint(const Vec2& p);
	void Offset(double x, double y);
	void Offset(const Vec2& v);
	void Offset(const Vec3& v);
	void SetAngle(double angle);
	double GetAngle()
	{
		return m_angle;
	}
	void Rotation(double angle, const Vec2& middle = Vec2::ZERO);
	bool IsCollision(const Polygon2D& rhs);
	void CheckBuildEdges();
	void Clear();
private:
	void BuildEdges();
	// Calculate the distance between [minA, maxA] and [minB, maxB]
	// The distance will be negative if the intervals overlap
	inline double IntervalDistance(double minA, double maxA, double minB,
	                               double maxB)
	{
		if (minA < minB)
		{
			return minB - maxA;
		}
		else
		{
			return minA - maxB;
		}
	}
	// Calculate the projection of a polygon on an axis and returns it as a [min, max] interval
	void ProjectPolygon(const Vec2& axis, const Polygon2D& polygon, double* min,
	                    double* max);
private:
	double  m_angle;
	Vec2s m_points, m_edges;
	bool    m_needBuildEdges;

};
typedef std::vector<Polygon2D> Polygon2Ds;
