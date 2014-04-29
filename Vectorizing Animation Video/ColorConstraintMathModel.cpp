
#include "ColorConstraintMathModel.h"

void ColorConstraintMathModel::AddPoint(double x, double y, const cv::Vec3b& p)
{
	m_NeedBuildModel = true;
	m_Pos.push_back(Vector2(x, y));
	m_Colors.push_back(Vector3(p[0], p[1], p[2]));
}

void ColorConstraintMathModel::AddPoint(double x, double y, const Vector3& p)
{
	m_NeedBuildModel = true;
	m_Colors.push_back(p);
}

void ColorConstraintMathModel::Clear()
{
	m_NeedBuildModel = true;
	m_Colors.clear();
	m_Pos.clear();
}

void Bounding(double& v, double vmin, double vmax)
{
	if (v < vmin)
	{
		v = vmin;
	}
	if (v > vmax)
	{
		v = vmax;
	}
}

Vector3 ColorConstraintMathModel::GetColorVector3(double x, double y)
{
	if (m_NeedBuildModel)
	{
		BuildModel();
	}
	if (m_Colors.size() > 6)
	{
		double rr = x1[0] + x1[1] * x + x1[2] * y + x1[3] * x * x + x1[4] * y * y + x1[5] * x * y;
		double gg = x2[0] + x2[1] * x + x2[2] * y + x2[3] * x * x + x2[4] * y * y + x2[5] * x * y;
		double bb = x3[0] + x3[1] * x + x3[2] * y + x3[3] * x * x + x3[4] * y * y + x3[5] * x * y;
		Bounding(rr, 0, 255);
		Bounding(gg, 0, 255);
		Bounding(bb, 0, 255);
		return Vector3(bb, gg, rr);
	}
	return Vector3(0, 0, 0);
}

cv::Vec3b ColorConstraintMathModel::GetColorCvPoint(double x, double y)
{
	if (m_NeedBuildModel)
	{
		BuildModel();
	}
	double rr = x1[0] + x1[1] * x + x1[2] * y + x1[3] * x * x + x1[4] * y * y + x1[5] * x * y;
	double gg = x2[0] + x2[1] * x + x2[2] * y + x2[3] * x * x + x2[4] * y * y + x2[5] * x * y;
	double bb = x3[0] + x3[1] * x + x3[2] * y + x3[3] * x * x + x3[4] * y * y + x3[5] * x * y;
	Bounding(rr, 0, 255);
	Bounding(gg, 0, 255);
	Bounding(bb, 0, 255);
	return cv::Vec3b(rr, gg, bb);
}

ColorConstraintMathModel::ColorConstraintMathModel(): m_NeedBuildModel(true)
{
}

void ColorConstraintMathModel::BuildModel()
{
	m_NeedBuildModel = false;
	if (!m_Colors.empty())
	{
		const int LEN = m_Colors.size();
		const int DIM = 6;
		MatrixXd A(LEN, DIM), At, Ata;
		VectorXd b1(LEN), b2(LEN), b3(LEN), Atb1, Atb2, Atb3;
		int c = 0;
		for (int i = 0; i < m_Colors.size(); ++i)
		{
			b1.coeffRef(c) = m_Colors[i][0];
			b2.coeffRef(c) = m_Colors[i][1];
			b3.coeffRef(c) = m_Colors[i][2];
			A.coeffRef(c, 0) = 1;
			A.coeffRef(c, 1) = m_Pos[i].x;
			A.coeffRef(c, 2) = m_Pos[i].y;
			A.coeffRef(c, 3) = m_Pos[i].x * m_Pos[i].x;
			A.coeffRef(c, 4) = m_Pos[i].y * m_Pos[i].y;
			A.coeffRef(c, 5) = m_Pos[i].x * m_Pos[i].y;
			c++;
		}
		At = A.transpose();
		Ata = At * A;
		Atb1 = At * b1;
		Atb2 = At * b2;
		Atb3 = At * b3;
		Eigen::PartialPivLU<MatrixXd> Atalu = Ata.lu();
		x1 = Atalu.solve(Atb1);
		x2 = Atalu.solve(Atb2);
		x3 = Atalu.solve(Atb3);
	}
}
