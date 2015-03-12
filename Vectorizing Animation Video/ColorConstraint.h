#pragma once
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "shared_ptr.h"
#include "ColorConstraintMedian.h"
#include "ColorConstraintAverage.h"
#include "ColorConstraintMathModel.h"
#include "ColorConstraintMyRBF.h"
SHARE_PTR(ColorConstraintMedian);
SHARE_PTR(ColorConstraintAverage);
SHARE_PTR(ColorConstraintMathModel);
SHARE_PTR(ColorConstraintMyRBF);

typedef ColorConstraintMyRBF ColorConstraint;
typedef ColorConstraintMyRBFs ColorConstraints;
typedef ColorConstraintMyRBF_sptr ColorConstraint_sptr;
typedef std::vector<ColorConstraint_sptr> ColorConstraint_sptrs;

struct Color2SideConstraint
{
	ColorConstraints left;
	ColorConstraints right;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& left;
		ar& right;
	}
};
