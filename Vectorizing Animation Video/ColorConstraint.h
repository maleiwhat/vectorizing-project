#pragma once
#include "shared_ptr.h"
#include "ColorConstraintMedian.h"
#include "ColorConstraintAverage.h"
SHARE_PTR(ColorConstraintMedian);
SHARE_PTR(ColorConstraintAverage);

typedef ColorConstraintAverage ColorConstraint;
typedef ColorConstraintAverage_sptr ColorConstraint_sptr;
typedef std::vector<ColorConstraint_sptr> ColorConstraint_sptrs;
