#include "GenerateDiffusion.h"

struct CurveMatching
{
	ints curve1;
	ints curve2;
};

CurveMatching ComputeMatching(FrameInfo& fi1, FrameInfo& fi2, cv::Vec2f move);
