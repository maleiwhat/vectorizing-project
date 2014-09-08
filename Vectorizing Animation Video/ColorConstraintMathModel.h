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
private:
	void BuildModel();
	Vector3s m_Colors;
	Vector2s m_Pos;
	bool m_NeedBuildModel;
	VectorXd x1q, x2q, x3q;
	VectorXd x1, x2, x3;
};
typedef std::vector<ColorConstraintMathModel> ColorConstraintMathModels;
