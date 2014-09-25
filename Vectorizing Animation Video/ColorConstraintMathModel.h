#pragma once

#include <opencv2\core\core.hpp>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <Eigen/LU>

typedef Eigen::SparseMatrix<double, 0>  MatrixXmp;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1>        VectorXmp;
typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::VectorXd VectorXd;

class ColorConstraintMathModel
{
public:
	ColorConstraintMathModel();
	void AddPoint(double x, double y, const cv::Vec3b& p);
	void AddPoint(double x, double y, const Vector3& p);
	bool CheckColor(double x, double y, const cv::Vec3b& p, double threshold);
	void Clear();
	Vector3 GetColorVector3(double x, double y);
	Vector3 GetColorVector3();
	cv::Vec3b GetColorCvPoint(double x, double y);
	int Size();
	void BuildModel();
private:
	Vector3s m_Colors;
	Vector2s m_Pos;
	bool m_NeedBuildModel;
	VectorXd x1q, x2q, x3q;
	VectorXd x1, x2, x3;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		{
			int s1 = x1.size(), s2 = x2.size(), s3 = x3.size();
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
			ar& s3;
			for(int i = 0; i < x3.size(); ++i)
			{
				ar& x3[i];
			}
		}
		{
			int s1 = x1q.size(), s2 = x2q.size(), s3 = x3q.size();
			ar& s1;
			for(int i = 0; i < x1q.size(); ++i)
			{
				ar& x1q[i];
			}
			ar& s2;
			for(int i = 0; i < x2q.size(); ++i)
			{
				ar& x2q[i];
			}
			ar& s3;
			for(int i = 0; i < x3q.size(); ++i)
			{
				ar& x3q[i];
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
			x3.resize(s2);
			for(int i = 0; i < x2.size(); ++i)
			{
				ar& x2[i];
			}
			ar& s3;
			x3.resize(s3);
			for(int i = 0; i < x3.size(); ++i)
			{
				ar& x3[i];
			}
		}
		{
			int s1, s2, s3;
			ar& s1;
			x1q.resize(s1);
			for(int i = 0; i < x1q.size(); ++i)
			{
				ar& x1q[i];
			}
			ar& s2;
			x3q.resize(s2);
			for(int i = 0; i < x2q.size(); ++i)
			{
				ar& x2q[i];
			}
			ar& s3;
			x3q.resize(s3);
			for(int i = 0; i < x3q.size(); ++i)
			{
				ar& x3q[i];
			}
		}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
};
typedef std::vector<ColorConstraintMathModel> ColorConstraintMathModels;
