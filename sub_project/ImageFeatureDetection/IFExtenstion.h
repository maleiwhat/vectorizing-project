#pragma once
#include "math\BasicMath.h"
double_vector Smoothing(const double_vector& data);
double_vector ConvertToAngle(const double_vector& data);
double_vector ConvertToSquareWave(const double_vector& data, int error, double value=100, double zero = 290);
bool	IsBlackLine(const double_vector& data, double zero = 290);
double_vector	GetBlackLine(const double_vector& data, double zero = 290);
bool	IsBrightLine(const double_vector& data, double zero = 290);
bool	IsShading(const double_vector& data, double zero = 290);
double_vector	ConvertToTest(const double_vector& data, double zero = 290);
double	GetVariance(const double_vector& data);
