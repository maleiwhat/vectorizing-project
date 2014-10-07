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
	Vector3 GetColorVector3Reverse(double x, double y);
	Vector3 GetColorVector3();
	cv::Vec3b GetColorCvPoint(double x, double y);
	int Size();
	void BuildModel();
	Vector3 AvgColor();
//private:
	Vector3s m_Colors;
	Vector2s m_Pos;
	Vector3	m_Median;
	int     m_ColorSize;
	bool m_NeedBuildModel;
	VectorXd x1q, x2q, x3q;
	VectorXd x1, x2, x3;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar& m_ColorSize;
		{
			int s1 = x1.size(), s2 = x2.size(), s3 = x3.size();
			ar& s1;
			for(int i = 0; i < x1.size(); ++i)
			{
				double v = x1[i];
				ar& v;
			}
			ar& s2;
			for(int i = 0; i < x2.size(); ++i)
			{
				double v = x2[i];
				ar& v;
			}
			ar& s3;
			for(int i = 0; i < x3.size(); ++i)
			{
				double v = x3[i];
				ar& v;
			}
		}
		{
			int s1 = x1q.size(), s2 = x2q.size(), s3 = x3q.size();
			ar& s1;
			for(int i = 0; i < x1q.size(); ++i)
			{
				double v = x1q[i];
				ar& v;
			}
			ar& s2;
			for(int i = 0; i < x2q.size(); ++i)
			{
				double v = x2q[i];
				ar& v;
			}
			ar& s3;
			for(int i = 0; i < x3q.size(); ++i)
			{
				double v = x3q[i];
				ar& v;
			}
		}
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		m_NeedBuildModel = false;
		ar& m_ColorSize;
		{
			int s1, s2, s3;
			ar& s1;
			x1.resize(s1);
			for(int i = 0; i < x1.size(); ++i)
			{
				double v;
				ar& v;
				x1[i] = v;
			}
			ar& s2;
			x2.resize(s2);
			for(int i = 0; i < x2.size(); ++i)
			{
				double v;
				ar& v;
				x2[i] = v;
			}
			ar& s3;
			x3.resize(s3);
			for(int i = 0; i < x3.size(); ++i)
			{
				double v;
				ar& v;
				x3[i] = v;
			}
		}
		{
			int s1, s2, s3;
			ar& s1;
			x1q.resize(s1);
			for(int i = 0; i < x1q.size(); ++i)
			{
				double v;
				ar& v;
				x1q[i] = v;
			}
			ar& s2;
			x2q.resize(s2);
			for(int i = 0; i < x2q.size(); ++i)
			{
				double v;
				ar& v;
				x2q[i] = v;
			}
			ar& s3;
			x3q.resize(s3);
			for(int i = 0; i < x3q.size(); ++i)
			{
				double v;
				ar& v;
				x3q[i] = v;
			}
		}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
};
typedef std::vector<ColorConstraintMathModel> ColorConstraintMathModels;
