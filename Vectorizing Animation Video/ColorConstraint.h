#pragma once
#include "shared_ptr.h"
#include "ColorConstraintMedian.h"
#include "ColorConstraintAverage.h"
SHARE_PTR(ColorConstraintMedian);
SHARE_PTR(ColorConstraintAverage);

typedef ColorConstraintMedian ColorConstraint;
typedef ColorConstraintMedian_sptr ColorConstraint_sptr;
typedef std::vector<ColorConstraint_sptr> ColorConstraint_sptrs;
