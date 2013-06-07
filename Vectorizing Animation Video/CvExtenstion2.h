#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Line.h"
#include "ColorConstraint.h"
#include "ImageSpline.h"
#include "Patch.h"

double_vector ComputeAngle(const Line& line);
double_vector Accumulation(const double_vector& line);
double_vector AbsAccumulation(const double_vector& line);
Line    GetControlPoint(const Line& line, double angle);
ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask,
                                 const cv::Mat& img);
cv::Vec3b& GetColor(cv::Mat& image, int x, int y);
Vector3s2d GetLinesColor(cv::Mat img, const Lines& lines);

struct Color2Side
{
	Vector3s2d left;
	Vector3s2d right;
};
Color2Side GetLinesColor2Side(cv::Mat img, const Lines& lines);
void OutputDiffusionCurve(std::string name, int w, int h, Color2Side& c2s,
                          Lines& lines);
cv::Mat MakeIsoSurfaceImg(cv::Mat img, int n);
ColorConstraint_sptr GetColorConstraint(cv::Mat& origin, cv::Mat& tmp);
