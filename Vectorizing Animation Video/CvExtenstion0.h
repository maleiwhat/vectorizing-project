#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Edge.h"
#include "Patch.h"
#include "LineFragment.h"
#include "PatchSpline.h"
#include "ImageSpline.h"
#include "ColorConstraint.h"
#include "CvExtenstion.h"
typedef std::vector<cv::Mat> Mats;
typedef std::map<int, int> int_int_map;
typedef std::map<int, double> int_double_map;
typedef std::vector<int_int_map> int_int_maps;
typedef std::vector<int_double_map> int_double_maps;

cv::Mat CannyEdge(cv::Mat& image, double threshold1 = 0, double threshold2 = 30,
				  int apertureSize = 3, bool L2gradient = false);
void    bwmorph_clean(cv::Mat& image);
bool    IsZero(cv::Mat& image, int i, int j);
Lines   ComputeEdgeLine(const cv::Mat& image);
Lines   ComputeEdgeLine2(const cv::Mat& image);
void    EdgeLink(cv::Mat& image, Line& now_line);
void    EdgeLink2(cv::Mat& image, Line& now_line);

void    Skeleton(cv::Mat& image);
Lines   ComputeTrappedBallEdge(cv::Mat& image, const Lines& old_line,
							   int ball_radius);
bool    LinkTrapBallBack(Line& li, const Vector2& ahead, cv::Mat& image,
						 int ball_radius);
void    cvThin(cv::Mat& src, cv::Mat& dst, int iterations = 1);
cv::Mat Laplace(const cv::Mat& image, int aperture_size = 3);
void    S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y,
					CvLines& out_array);
void    S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y,
					CvPatchs& out_array, int dilation = 0, int erosion = 0);
CvLines GetSidelines(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image0, int dilation, int erosion);
void GetSkeletonLine(cv::Mat bmap, Lines& lines, double_vector2d& linewidths11);
void GetMatrixf(int w, int h, floatptrs& ary, int x, int y, cv::Mat& img);

cv::Mat TrapBallMaskAll(cv::Mat image, cv::Mat oriImg);
cv::Mat TrapBallMask1(cv::Mat image, int size, int moprh = cv::MORPH_ELLIPSE);
cv::Mat TrapBallMask2(cv::Mat image, int size, int moprh = cv::MORPH_ELLIPSE);
cv::Mat TrapBallMask3(cv::Mat image, int size, int moprh = cv::MORPH_ELLIPSE, int maxadd = 9999);
cv::Mat TrapBallMask4(cv::Mat image, int moprh = cv::MORPH_ELLIPSE);
cv::Mat TrapBallMask5(cv::Mat image, int size, int moprh, cv::Mat oriImg, int& cstart);
cv::Mat FixSpaceMask(cv::Mat image);
cv::Mat MixTrapBallMask(cv::Mat image, cv::Mat oriImg, double threshold1, double threshold2);
cv::Mat ConvertToMedian(cv::Mat image, cv::Mat src);
cv::Mat ConvertToIndex(cv::Mat src, cv::Mat ori, ColorConstraints& output);
void    S5ReColor(cv::Mat& image);
void    S6ReColor(cv::Mat& image, cv::Mat& oimg, ColorConstraints& ccms);
Mats    S7ReColor(cv::Mat& image, cv::Mat& oimg, ColorConstraints& ccms);

bool CheckMaskImg1(cv::Mat image, cv::Mat mask, int x, int y);
bool CheckMaskImg2(cv::Mat image, cv::Mat mask, int x, int y);
bool CheckMaskImg3(cv::Mat image, cv::Mat maskbig, cv::Mat mask, int x, int y);
void MaskImgDraw(cv::Mat image, cv::Mat mask, int x, int y, cv::Vec3b c);
cv::Vec3b MaskImgGet(cv::Mat image, cv::Mat mask, int x, int y);

cv::Mat WhiteBalance(cv::Mat oriImg);
cv::Mat WhiteBalance2(cv::Mat oriImg);
cv::Mat ImgSharpen(cv::Mat oriImg);
void GetDbDrMbMr(cv::Mat oriImg, double& Db, double& Dr, double& Mb, double& Mr);
