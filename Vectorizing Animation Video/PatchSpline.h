#pragma once
#include <vector>
#include "Edge.h"
#include "LineFragment.h"

// 每一個PatchLine裡面有很多的LineFragments
// 裡面記錄了相對關係，以方便後面的整合
class PatchLine
{
public:
	PatchLine(void);
	~PatchLine(void);
	int m_id;
	LineFragments m_LineFragments;
};
typedef std::vector<PatchLine> PatchLines;

struct LineIndex
{
	int m_id;
	// 正向 還是 逆向
	bool m_Forward;
};

typedef std::vector<LineIndex> LineIndexs;
class PatchSpline
{
public:
	// 記錄由哪幾個Spline連起來的這些Spline的index
	LineIndexs  m_LineIndexs;
};
typedef std::vector<PatchSpline> PatchSplines;
typedef std::vector<PatchSplines> PatchSplines2d;
