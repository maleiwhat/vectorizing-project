#pragma once
#ifdef USE_CGAL
#pragma warning( push, 0 )
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#pragma warning( pop )
#endif // USE_CGAL
#include <memory.h>
#include <vector>
#include <opencv2/core/core.hpp>
#include <deque>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "math/Vector2.h"
#include "math/Vector3.h"

#ifdef USE_CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel CgalInexactKernel;
typedef CgalInexactKernel::Point_2 CgalPoint;
typedef std::vector<CgalPoint> CgalLine;
typedef std::vector<CgalLine> CgalLines;
#endif // USE_CGAL

typedef std::vector<Vector2> Line;
typedef std::vector<Line> Lines;
typedef std::vector<cv::Point> CvLine;
typedef std::vector<CvLine> CvLines;
struct LineSeg
{
	Vector2 beg, end;
	LineSeg() {}
	LineSeg(Vector2& b, Vector2& e): beg(b), end(e) {}
};
typedef std::deque<LineSeg> LineSegs;

struct Endpoint
{
	Vector2 pos;
	int idx1, idx2;
	double  angle;
	Endpoint() {}
	Endpoint(const Vector2& p, int i1, int i2, double a)
		: pos(p), idx1(i1), idx2(i2), angle(a) {}
	bool operator==(const Endpoint& rhs)
	{
		return idx1 == rhs.idx1 && idx2 == rhs.idx2;
	}
	bool operator!=(const Endpoint& rhs)
	{
		return idx1 != rhs.idx1 || idx2 != rhs.idx2;
	}
};
typedef std::vector<Endpoint> Endpoints;

struct LineEnd
{

	enum LinkMethod
	{
		NOTHING = 0,
		BEGIN_TO_BEGIN = 0x01,
		BEGIN_TO_END = 0x02,
		END_TO_BEGIN = 0x04,
		END_TO_END = 0x08,
	};
	LineEnd*  linkIdx1;
	LineEnd*  linkIdx2;
	Vector2 beg;
	Vector2 end;
	// angleBeg is p[0]-p[2]
	// angleEnd is p[last]-p[last-2]
	int idx1, idx2;
	double angleBeg, angleEnd;
	LineEnd* beglink;
	LineEnd* endlink;
	LineEnd()
	{
		memset(this, 0, sizeof(LineEnd));
	}
	LineEnd(const Vector2& p1, const Vector2& p2, int i1, int i2, double a1, double a2)
		: beg(p1), end(p2), idx1(i1), idx2(i2), angleBeg(a1), angleEnd(a2),
		  linkIdx1(0), linkIdx2(0), beglink(0), endlink(0) {}
};
typedef std::vector<LineEnd> LineEnds;

struct Color2Side
{
	Vector3s2d left;
	Vector3s2d right;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & left;
		ar & right;
	}
};
