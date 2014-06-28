#pragma once
#include "shared_ptr.h"
#include "ColorConstraintMedian.h"
#include "ColorConstraintAverage.h"
#include "ColorConstraintMathModel.h"
SHARE_PTR(ColorConstraintMedian);
SHARE_PTR(ColorConstraintAverage);
SHARE_PTR(ColorConstraintMathModel);

typedef ColorConstraintMathModel ColorConstraint;
typedef ColorConstraintMathModels ColorConstraints;
typedef ColorConstraintMathModel_sptr ColorConstraint_sptr;
typedef std::vector<ColorConstraint_sptr> ColorConstraint_sptrs;
