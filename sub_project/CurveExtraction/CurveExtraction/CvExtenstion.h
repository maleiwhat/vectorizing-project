#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Edge.h"
#include "Patch.h"
#include "LineFragment.h"
#include "PatchSpline.h"
#include "ImageSpline.h"

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
// �X��
void    Dilation(cv::Mat& image, int dilation_elem = 2, int dilation_size = 2);
// �Ӥ�
void    Erosion(cv::Mat& image, int erosion_elem, int erosion_size);
void    cvThin(cv::Mat& src, cv::Mat& dst, int iterations = 1);

cv::Mat Laplace(const cv::Mat& image, int aperture_size = 3);
void    S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y,
                    CvPoints2d& out_array);
void    S1FloodFill(cv::Mat& image,  cv::Mat& mask, int x, int y,
                    CvPatchs& out_array, int dilation = 0, int erosion = 0);
void    S2FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, cv::Mat mask02,
                    int range, int x, int y, CvPatchs& out_array, int dilation = 0,
                    int erosion = 0);
void    S2FloodFill(cv::Mat& image, cv::Mat& mask01, cv::Mat mask02, int range,
                    int x, int y, int dilation = 0, int erosion = 0);
void    LineFloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y);
CvPoints2d GetSidelines(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image);
CvPatchs S1GetPatchs(const cv::Mat& image0, int dilation, int erosion);
CvPatchs S2GetPatchs(const cv::Mat& image, int dilation = 0, int erosion = 0);
ImageSpline S3GetPatchs(const cv::Mat& image0, int dilation, int erosion);
ImageSpline GetImageSpline(CvPatchs& patchs, Lines& lines, cv::Mat lineImage);
PatchLines GetPatchSplines(CvPatchs& patchs, cv::Mat& patchImage);
Lines GetAllLineFromLineImage(cv::Mat& lineImage);
void    FixHole(cv::Mat& patchImage);
Lines   GetLines(const CvPoints2d& cvp);
cv::Mat MakeLineImage(const cv::Mat& image, const Lines& lines);
cv::Mat MakeLineImage(const cv::Mat& image, Patch& patch);
cv::Vec3b& GetColor(cv::Mat& image, int x, int y);

inline bool CorrectPosition(cv::Mat& image, int x, int y)
{
	if (x >= 0 && y >= 0 && x < image.cols && y < image.rows)
	{
		return true;
	}

	return false;
}

void ClearEdge(cv::Mat& image);
void EdgeLink_LineFragment(cv::Mat& image, Line& now_line, int now_value);
void AddCathetus(CvPatchs& cvps, cv::Mat timage);