#pragma once
#include <vector>
#include "Edge.h"
#include "LineFragment.h"

// �C�@��PatchLine�̭����ܦh��LineFragments
// �̭��O���F�۹����Y�A�H��K�᭱����X
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
	// ���V �٬O �f�V
	bool m_Forward;
};

typedef std::vector<LineIndex> LineIndexs;
class PatchSpline
{
public:
	// �O���ѭ��X��Spline�s�_�Ӫ��o��Spline��index
	LineIndexs  m_LineIndexs;
};
typedef std::vector<PatchSpline> PatchSplines;
typedef std::vector<PatchSplines> PatchSplines2d;
