#pragma once
#include "LineDef.h"

Lines   GetLines(const CvLines& cvp, double xOffset = 0, double yOffset = 0);
Lines   GetLines(const Lines& cvp, double xOffset = 0, double yOffset = 0);
Lines   GetLines(const CgalLines& cvp, double xOffset = 0, double yOffset = 0);
Line    GetLine(const CvLine& cvp, double xOffset = 0, double yOffset = 0);
Line    GetLine(const Line& cvp, double xOffset = 0, double yOffset = 0);
Line    GetLine(const CgalLine& cvp, double xOffset = 0, double yOffset = 0);

CvLines GetCvLines(const Lines& cvp, double xOffset = 0, double yOffset = 0);
CvLines GetCvLines(const CgalLines& cvp, double xOffset = 0,
				   double yOffset = 0);
CvLine  GetCvLine(const Line& cvp, double xOffset = 0, double yOffset = 0);
CvLine  GetCvLine(const CgalLine& cvp, double xOffset = 0, double yOffset = 0);

CgalLines   GetCgalLines(const CvLines& cvp, double xOffset = 0,
						 double yOffset = 0);
CgalLines   GetCgalLines(const Lines& cvp, double xOffset = 0,
						 double yOffset = 0);
CgalLine    GetCgalLine(const CvLine& cvp, double xOffset = 0,
						double yOffset = 0);
CgalLine    GetCgalLine(const Line& cvp, double xOffset = 0,
						double yOffset = 0);

Line    SmoothingLen5(const Line& cvp, double centroidRadio = 1.0,
					  int repeat = 1);
Line    SmoothingLen3(const Line& cvp, double centroidRadio = 1.0,
					  int repeat = 1);
Lines   SmoothingLen5(const Lines& cvp, double centroidRadio = 1.0,
					  int repeat = 1);
Lines   SmoothingLen3(const Lines& cvp, double centroidRadio = 1.0,
					  int repeat = 1);
Vector3s    SmoothingLen5(const Vector3s& cvp, double centroidRadio = 1.0,
					  int repeat = 1);
Vector3s2d   SmoothingLen5(const Vector3s2d& cvp, double centroidRadio = 1.0,
					  int repeat = 1);

Line    GetNormalsLen2(const Line& cvp);
Line    GetNormalsLen3(const Line& cvp);
Lines   GetNormalsLen2(const Lines& cvp);
Lines   GetNormalsLen3(const Lines& cvp);
Line    LineAdd(const Line& aLine, const Line& bLine);
Lines   LinesAdd(const Lines& aLine, const Lines& bLine);
Line    LineSub(const Line& aLine, const Line& bLine);
Lines   LinesSub(const Lines& aLine, const Lines& bLine);

Line    FixLineWidths(const Line& aLine, int range);
Lines   FixLineWidths(const Lines& aLine, int range);

Line    CleanOrphanedLineWidths(const Line& aLine, int num);
Lines   CleanOrphanedLineWidths(const Lines& aLine, int num);

Line    SmoothingHas0Len5(const Line& cvp, double centroidRadio = 1.0,
						  int repeat = 1);
Line    SmoothingHas0Len3(const Line& cvp, double centroidRadio = 1.0,
						  int repeat = 1);
Lines   SmoothingHas0Len5(const Lines& cvp, double centroidRadio = 1.0,
						  int repeat = 1);
Lines   SmoothingHas0Len3(const Lines& cvp, double centroidRadio = 1.0,
						  int repeat = 1);

Lines   SplitStraightLine(const Line& cvp, double angle);
Lines   SplitStraightLine(const Lines& cvp, double angle);

typedef std::pair<Lines, Lines> LinesPair;
LinesPair   SplitStraightLineAndWidth(const Line& cvp, const Line& width,
									  double angle);
LinesPair   SplitStraightLineAndWidth(const Lines& cvp, const Lines& width,
									  double angle);

double_vector ComputeAngle(const Line& line);
Line    GetControlPoint(const Line& line, double angle);

Endpoints   GetEndpoints(const Lines& cvp);
bool LessEndpointX(const Endpoint& a, const Endpoint& b);
bool GreaterEndpointX(const Endpoint& a, const Endpoint& b);
bool LessEndpointY(const Endpoint& a, const Endpoint& b);
bool GreaterEndpointY(const Endpoint& a, const Endpoint& b);
void        SortEndpointsX(Endpoints& cvp);
Endpoints   FindNearEndpoints(const Endpoints& cvp, const Vector2& pos, double distance);
Endpoints   FindNearSortedXEndpoints(const Endpoints& cvp, const Vector2& pos, double distance);
bool CheckEndpointSimilarity(const Endpoint& lhs, const Endpoint& rhs, double angle);
void ConnectSimilarLines(Lines& pos, Lines& width, double angle);
double  GetLineWidthPercent(const Line& cvp);
void    ClearLineWidthByPercent(Lines& widths, double angle);

LineEnds GetLineEnds(const Lines& cvp);
void LinkLineEnds(LineEnds& les, double distance, double angle);
bool CheckLinkEndSimilarity(const LineEnd& lhs, const LineEnd& rhs,
							LineEnd::LinkMethod c, double angle);
void ConnectSimilarLines(const LineEnds& les, Lines& pos, Lines& width);
LineEnds FindLinkHead(const LineEnds& lhs, int lineNum);
void ConnectNearestLines(const LineEnds& les, Lines& pos, Lines& width, double d1, double d2,
						 double angle);
void IncreaseDensity(Line& pos, Line& pos2);
void IncreaseDensity(Lines& pos, Lines& pos2);
void FixBeginWidth(Line& width, int len);
void FixEndWidth(Line& width, int len);

