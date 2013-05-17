#pragma once
#include "Line.h"

double_vector ComputeAngle(const Line& line);
double_vector Accumulation(const double_vector& line);
double_vector AbsAccumulation(const double_vector& line);
Line    GetControlPoint(const Line& line, double angle);

