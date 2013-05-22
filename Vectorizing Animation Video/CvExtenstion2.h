#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Line.h"
#include "ColorConstraintMedian.h"
#include "ImageSpline.h"
#include "Patch.h"

double_vector ComputeAngle(const Line& line);
double_vector Accumulation(const double_vector& line);
double_vector AbsAccumulation(const double_vector& line);
Line    GetControlPoint(const Line& line, double angle);
ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask, const cv::Mat& img);

