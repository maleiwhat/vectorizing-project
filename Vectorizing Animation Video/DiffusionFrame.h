#pragma once
#include <vector>
#include "LineDef.h"
#include "math\Vector3.h"


class DiffusionFrame
{
public:
	DiffusionFrame(void);
	~DiffusionFrame(void);
	Lines m_BlackLine;
	Lines m_BLineWidth;
	Vector3s2d lineColors;
	Lines diffusionConstrant;
	Vector3s2d colors;
	Lines m_BlackLine2;
	Color2Side color2s;
};
typedef std::vector<DiffusionFrame> DiffusionFrames;
