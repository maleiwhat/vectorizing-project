#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Edge.h"
#include "Patch.h"
#include "LineFragment.h"
#include "PatchSpline.h"
#include "ImageSpline.h"

cv::Mat	CannyEdge(cv::Mat& image, double threshold1 = 0, double threshold2 = 30, int apertureSize = 3, bool L2gradient = false);
void	bwmorph_clean(cv::Mat& image);
bool	IsZero(cv::Mat& image, int i, int j);
Lines	ComputeEdgeLine(const cv::Mat& image);
Lines	ComputeEdgeLine2(const cv::Mat& image);
void	EdgeLink(cv::Mat& image, Line& now_line);
void	EdgeLink2(cv::Mat& image, Line& now_line);

void	Skeleton(cv::Mat& image);
Lines	ComputeTrappedBallEdge(cv::Mat& image, const Lines& old_line, int ball_radius);
bool	LinkTrapBallBack(Line& li, const Vector2& ahead, cv::Mat& image, int ball_radius);
void	cvThin(cv::Mat& src, cv::Mat& dst, int iterations = 1);
cv::Mat Laplace(const cv::Mat& image, int aperture_size = 3);
void	S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y, CvPoints2d& out_array);
void	S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y, CvPatchs& out_array, int dilation = 0, int erosion = 0);
CvPoints2d GetSidelines(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image0, int dilation, int erosion);
void GetSkeletonLine(cv::Mat bmap, Lines& lines, double_vector2d& linewidths);
void GetMatrixf(int w, int h, floatptrs& ary, int x, int y, cv::Mat& img);