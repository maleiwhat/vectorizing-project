#pragma once
#include "LineDef.h"

#ifdef USE_CGAL
CvLines GetCvLines(const CgalLines& cvp, double xOffset = 0, double yOffset = 0);
CvLine  GetCvLine(const CgalLine& cvp, double xOffset = 0, double yOffset = 0);
Lines   GetLines(const CgalLines& cvp, double xOffset = 0, double yOffset = 0);
Line    GetLine(const CgalLine& cvp, double xOffset = 0, double yOffset = 0);
CgalLines   GetCgalLines(const CvLines& cvp, double xOffset = 0,
						 double yOffset = 0);
CgalLines   GetCgalLines(const Lines& cvp, double xOffset = 0,
						 double yOffset = 0);
CgalLine    GetCgalLine(const CvLine& cvp, double xOffset = 0,
						double yOffset = 0);
CgalLine    GetCgalLine(const Line& cvp, double xOffset = 0,
						double yOffset = 0);
#endif // USE_CGAL

Lines   GetLines(const CvLines& cvp, double xOffset = 0, double yOffset = 0);
Lines   GetLines(const Lines& cvp, double xOffset = 0, double yOffset = 0);

Line    GetLine(const CvLine& cvp, double xOffset = 0, double yOffset = 0);
Line    GetLine(const Line& cvp, double xOffset = 0, double yOffset = 0);

CvLines GetCvLines(const Lines& cvp, double xOffset = 0, double yOffset = 0);
CvLine  GetCvLine(const Line& cvp, double xOffset = 0, double yOffset = 0);

Line    SmoothingLen5(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Line    SmoothingLen3(const Line& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines   SmoothingLen5(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);
Lines   SmoothingLen3(const Lines& cvp, double centroidRadio = 1.0, int repeat = 1);
Vector3s    SmoothingLen5(const Vector3s& cvp, double centroidRadio = 1.0, int repeat = 1);
Vector3s2d   SmoothingLen5(const Vector3s2d& cvp, double centroidRadio = 1.0, int repeat = 1);

ints2d    MedianLen5(const ints2d& cvp, int repeat = 1);
ints    MedianLen5(const ints& cvp, int repeat = 1);

Vector3s    MedianLen5(const Vector3s& cvp, int repeat = 1);
Vector3s2d    MedianLen5(const Vector3s2d& cvp, int repeat = 1);
Vector3s    MedianLen(const Vector3s& cvp, int len, int repeat = 1);
Vector3s2d    MedianLen(const Vector3s2d& cvp, int len, int repeat = 1);

Line    GetNormalsLen2(const Line& cvp);
Line    GetNormalsLen3(const Line& cvp);
Lines   GetNormalsLen2(const Lines& cvp);
Lines   GetNormalsLen3(const Lines& cvp);

Line    LineAdd(const Line& aLine, const Line& bLine);
Lines   LinesAdd(const Lines& aLine, const Lines& bLine);
Line    LineSub(const Line& aLine, const Line& bLine);
Lines   LinesSub(const Lines& aLine, const Lines& bLine);

Vector3s    HistormMappingLight(const Vector3s& color, double radio);
Vector3s2d  HistormMappingLight(const Vector3s2d& color, double radio);
Vector3s    HistormMappingHSV(const Vector3s& color, double radio);

Vector3s    FixLineColors(const Vector3s& aLine, int range, int findlimit);
Vector3s2d  FixLineColors(const Vector3s2d& aLine, int range, int findlimit);

Line    FixLineWidths(const Line& aLine, int range);
Lines   FixLineWidths(const Lines& aLine, int range);

ints    FixIndexs(const ints& aLine, int range);
ints2d  FixIndexs(const ints2d& aLine, int range);

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
Vector3s    SmoothingHas0Len5(const Vector3s& cvp, double centroidRadio = 1.0,
							  int repeat = 1);
Vector3s2d   SmoothingHas0Len5(const Vector3s2d& cvp, double centroidRadio = 1.0,
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
void MarkHasLink_LineEnds(LineEnds& les, const Lines& cvp);

Acutes LinkLineAcutes(LineEnds& les, double distance, double angle);
void LinkLineEnds180(LineEnds& les, double distance, double angle);
void LinkLineEnds(LineEnds& les, double distance, double angle);
bool CheckLinkEnd_Similarity180(const LineEnd& lhs, const LineEnd& rhs,
							LineEnd::LinkMethod c, double angle);
bool CheckLinkEnd_Similarity(const LineEnd& lhs, const LineEnd& rhs,
							LineEnd::LinkMethod c, double angle);
void ConnectLineEnds(LineEnds& les, Lines& pos, Lines& width);
void ConnectLineEnds2(const LineEnds& les, Lines& pos, Lines& width);
void ConnectLineEnds3(const LineEnds& les, Lines& pos, Lines& width);
void ConnectSimilarColor2Side(const LineEnds& les, Lines& pos, Color2Side& width);

void ClearJointArea(const LineEnds& les, Lines& pos, Color2Side& color2s, double len);
void ConnectNearestLines(const LineEnds& les, Lines& pos, Lines& width, double d2,
						 double angle);
void ConnectNearestLines(const LineEnds& les, Lines& pos, Color2Side& width, double d2,
						 double angle);
void IncreaseDensity(Line& pos, Line& pos2);
void IncreaseDensity(Lines& pos, Lines& pos2);
void FixBeginWidth(Line& width, int len);
void FixEndWidth(Line& width, int len);
void FixBeginWidth(Vector3s& width, int len);
void FixEndWidth(Vector3s& width, int len);

bool intersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2);
bool CheckAngle(double a1, double a2, double limitA);

