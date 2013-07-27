#pragma once
#include "math\BasicMath.h"

double_vector SmoothingLoop(const double_vector& data,
							double centroidRadio = 1.0, int repeat = 1);
double_vector SmoothingLen3(const double_vector& data,
							double centroidRadio = 1.0, int repeat = 1);
double_vector SmoothingLen5(const double_vector& data,
							double centroidRadio = 1.0, int repeat = 1);
double_vector ConvertToAngle(const double_vector& data, double zero = 290);
double_vector LogSmooth(const double_vector& data, double zero = 290);
double_vector ConvertToSquareWave(const double_vector& data, int error,
								  double value = 100, double zero = 290);

bool	IsBlackLine(const double_vector& data, double zero = 290);
bool	IsBrightLine(const double_vector& data, double zero = 290);
bool	IsShading(const double_vector& data, double zero = 290);
double_vector	GetBlackLine(const double_vector& data, double zero = 290);
double_vector	GetLineWidth(const double_vector& data, double lineWidth,
							 double zero = 290);
double	GetVariance(const double_vector& data);
