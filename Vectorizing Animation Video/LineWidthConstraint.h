#pragma once
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <Eigen/LU>

typedef std::vector<double> doubles;
typedef Eigen::SparseMatrix<double, 0>  MatrixXmp;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1>        VectorXmp;
typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::VectorXd VectorXd;

class LineWidthConstraint
{
public:
	LineWidthConstraint(void);
	~LineWidthConstraint(void);
public:
	void AddPoint(double x, double y, double w1, double w2);
	void Clear();
	Vector3 GetColorVector3(double dis);
	double Length();
	void Scale0to1();
	void BuildModel();
private:
	double m_len;
	doubles m_Widths1;
	doubles m_Widths2;
	Vector2s m_Pos;
	bool m_NeedBuildModel;
	VectorXd x1, x2;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		{
			int s1 = x1.size(), s2 = x2.size();
			ar& s1;
			for(int i = 0; i < x1.size(); ++i)
			{
				ar& x1[i];
			}
			ar& s2;
			for(int i = 0; i < x2.size(); ++i)
			{
				ar& x2[i];
			}
		}
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		m_NeedBuildModel = false;
		{
			int s1, s2, s3;
			ar& s1;
			x1.resize(s1);
			for(int i = 0; i < x1.size(); ++i)
			{
				ar& x1[i];
			}
			ar& s2;
			x2.resize(s2);
			for(int i = 0; i < x2.size(); ++i)
			{
				ar& x2[i];
			}
		}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
};
typedef std::vector<LineWidthConstraint> LineWidthConstraints;
