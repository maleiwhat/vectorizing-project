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

struct LineEnd;
typedef std::vector<char> chars;
typedef std::vector<int> ints;
typedef std::deque<int> intds;
typedef std::vector<unsigned int> uints;
typedef std::vector<LineEnd*> LineEnd_ptrs;
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
	int idx, len;
	double angleBeg, angleEnd;
	LineEnd* beglink;
	LineEnd* endlink;
	ints beglinks;
	ints endlinks;
	bool haslink1, haslink2;
	LineEnd()
	{
		memset(this, 0, sizeof(LineEnd));
	}
	LineEnd(const Vector2& p1, const Vector2& p2, int i1, int i2, double a1, double a2)
		: beg(p1), end(p2), idx(i1), len(i2), angleBeg(a1), angleEnd(a2),
		  linkIdx1(0), linkIdx2(0), beglink(0), endlink(0), haslink1(0), haslink2(0) {}
};
typedef std::vector<LineEnd> LineEnds;

struct LineEndPoint
{
	enum BEGIN_END
	{
		BEGIN,
		END
	};
	LineEnd le;
	BEGIN_END dir;
};
typedef std::vector<LineEndPoint> LineEndPoints;
struct Acute
{
	LineEndPoints leps;
	Vector2 pos;
};
typedef std::vector<Acute> Acutes;

struct Color2Side
{
	Vector3s2d left;
	Vector3s2d right;
	void reverse()
	{
		std::reverse(left.begin(), left.end());
		std::reverse(right.begin(), right.end());
	}
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& left;
		ar& right;
	}
};

typedef std::vector<int> ints;
typedef std::vector<ints> ints2d;
typedef std::vector<bool> bools;

struct Index2Side
{
	ints2d left;
	ints2d right;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& left;
		ar& right;
	}
};

