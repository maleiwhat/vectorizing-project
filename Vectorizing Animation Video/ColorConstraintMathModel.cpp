
#include "ColorConstraintMathModel.h"

const int THRESHOLD = 20;
const int THRESHOLD2 = 20;

void ColorConstraintMathModel::AddPoint(double x, double y, const cv::Vec3b& p)
{
	m_NeedBuildModel = true;
	m_Pos.push_back(Vector2(x, y));
	m_Colors.push_back(Vector3(p[0], p[1], p[2]));
	m_ColorSize++;
}

void ColorConstraintMathModel::AddPoint(double x, double y, const Vector3& p)
{
	m_NeedBuildModel = true;
	m_Colors.push_back(p);
	m_Pos.push_back(Vector2(x, y));
	m_ColorSize++;
}

void ColorConstraintMathModel::Clear()
{
	m_NeedBuildModel = true;
	m_Colors.clear();
	m_Pos.clear();
	m_ColorSize = 0;
}

void Bounding(double& v, double vmin, double vmax)
{
	if(v < vmin)
	{
		v = vmin;
	}
	if(v > vmax)
	{
		v = vmax;
	}
}

Vector3 ColorConstraintMathModel::GetColorVector3(double x, double y)
{
	if(m_NeedBuildModel)
	{
		BuildModel();
	}
	if(m_ColorSize > THRESHOLD)
	{
		double rr1 = x1q[0] + x1q[1] * x + x1q[2] * y + x1q[3] * x * x + x1q[4] * y * y + x1q[5] * x * y;
		double gg1 = x2q[0] + x2q[1] * x + x2q[2] * y + x2q[3] * x * x + x2q[4] * y * y + x2q[5] * x * y;
		double bb1 = x3q[0] + x3q[1] * x + x3q[2] * y + x3q[3] * x * x + x3q[4] * y * y + x3q[5] * x * y;
		double rr2 = x1[0] + x1[1] * x + x1[2] * y;
		double gg2 = x2[0] + x2[1] * x + x2[2] * y;
		double bb2 = x3[0] + x3[1] * x + x3[2] * y;
		rr1 = (rr1 + rr2) * 0.5;
		gg1 = (gg1 + gg2) * 0.5;
		bb1 = (bb1 + bb2) * 0.5;
		Bounding(rr1, 0, 255);
		Bounding(gg1, 0, 255);
		Bounding(bb1, 0, 255);
		return Vector3(rr1, gg1, bb1);
	}
	else if(m_ColorSize > THRESHOLD2)
	{
		double rr2 = x1[0] + x1[1] * x + x1[2] * y;
		double gg2 = x2[0] + x2[1] * x + x2[2] * y;
		double bb2 = x3[0] + x3[1] * x + x3[2] * y;
		Bounding(rr2, 0, 255);
		Bounding(gg2, 0, 255);
		Bounding(bb2, 0, 255);
		return Vector3(rr2, gg2, bb2);
	}
	else if(m_Colors.size() > 0)
	{
		double rr = 0;
		double gg = 0;
		double bb = 0;
		for(int i = 0; i < m_Colors.size(); ++i)
		{
			rr += m_Colors[i].x;
			gg += m_Colors[i].y;
			bb += m_Colors[i].z;
		}
		rr /= m_Colors.size();
		gg /= m_Colors.size();
		bb /= m_Colors.size();
		Bounding(rr, 0, 255);
		Bounding(gg, 0, 255);
		Bounding(bb, 0, 255);
		return Vector3(rr, gg, bb);
	}
	return Vector3(255, 0, 0);
}

Vector3 ColorConstraintMathModel::GetColorVector3()
{
	if(m_Colors.empty())
	{
		return Vector3(255, 255, 255);
	}
	else
	{
		return m_Colors[0];
	}
}

cv::Vec3b ColorConstraintMathModel::GetColorCvPoint(double x, double y)
{
	if(m_NeedBuildModel)
	{
		BuildModel();
	}
	if(m_Colors.size() > THRESHOLD)
	{
		double rr = x1q[0] + x1q[1] * x + x1q[2] * y + x1q[3] * x * x + x1q[4] * y * y + x1q[5] * x * y;
		double gg = x2q[0] + x2q[1] * x + x2q[2] * y + x2q[3] * x * x + x2q[4] * y * y + x2q[5] * x * y;
		double bb = x3q[0] + x3q[1] * x + x3q[2] * y + x3q[3] * x * x + x3q[4] * y * y + x3q[5] * x * y;
		Bounding(rr, 0, 255);
		Bounding(gg, 0, 255);
		Bounding(bb, 0, 255);
		return cv::Vec3b(rr, gg, bb);
	}
	else if(m_Colors.size() > THRESHOLD2)
	{
		double rr = x1[0] + x1[1] * x + x1[2] * y;
		double gg = x2[0] + x2[1] * x + x2[2] * y;
		double bb = x3[0] + x3[1] * x + x3[2] * y;
		Bounding(rr, 0, 255);
		Bounding(gg, 0, 255);
		Bounding(bb, 0, 255);
		return cv::Vec3b(rr, gg, bb);
	}
	else if(m_Colors.size() > 0)
	{
		double rr = 0;
		double gg = 0;
		double bb = 0;
		for(int i = 0; i < m_Colors.size(); ++i)
		{
			rr += m_Colors[i].x;
			gg += m_Colors[i].y;
			bb += m_Colors[i].z;
		}
		rr /= m_Colors.size();
		gg /= m_Colors.size();
		bb /= m_Colors.size();
		Bounding(rr, 0, 255);
		Bounding(gg, 0, 255);
		Bounding(bb, 0, 255);
		return cv::Vec3b(rr, gg, bb);
	}
	return cv::Vec3b(255, 0, 0);
}

ColorConstraintMathModel::ColorConstraintMathModel(): m_NeedBuildModel(true)
{
	m_ColorSize = 0;
}

void ColorConstraintMathModel::BuildModel()
{
	m_NeedBuildModel = false;
	if(m_ColorSize > THRESHOLD)
	{
		const int LEN = m_Colors.size();
		const int DIM = 6;
		MatrixXd A(LEN, DIM), At, Ata;
		VectorXd b1(LEN), b2(LEN), b3(LEN), Atb1, Atb2, Atb3;
		int c = 0;
		for(int i = 0; i < m_Colors.size(); ++i)
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
		x1q = Atalu.solve(Atb1);
		x2q = Atalu.solve(Atb2);
		x3q = Atalu.solve(Atb3);
	}
	if(m_ColorSize > THRESHOLD2)
	{
		const int LEN = m_Colors.size();
		const int DIM = 3;
		MatrixXd A(LEN, DIM), At, Ata;
		VectorXd b1(LEN), b2(LEN), b3(LEN), Atb1, Atb2, Atb3;
		int c = 0;
		for(int i = 0; i < m_Colors.size(); ++i)
		{
			b1.coeffRef(c) = m_Colors[i][0];
			b2.coeffRef(c) = m_Colors[i][1];
			b3.coeffRef(c) = m_Colors[i][2];
			A.coeffRef(c, 0) = 1;
			A.coeffRef(c, 1) = m_Pos[i].x;
			A.coeffRef(c, 2) = m_Pos[i].y;
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

bool ColorConstraintMathModel::CheckColor(double x, double y, const cv::Vec3b& p, double threshold)
{
	Vector3 test = GetColorVector3(x, y);
	test.x -= p[0];
	test.y -= p[1];
	test.z -= p[2];
	if(test.squaredLength() < threshold * threshold)
	{
		return true;
	}
	return false;
}

int ColorConstraintMathModel::Size()
{
	return m_Colors.size();
}

Vector3 ColorConstraintMathModel::GetColorVector3Reverse(double x, double y)
{
	Vector3 tmp = GetColorVector3(x, y);
	return Vector3(tmp[2], tmp[1], tmp[0]);
}

Vector3 ColorConstraintMathModel::AvgColor()
{
	Vector3 sum;
	for(int i = 0; i < m_Colors.size(); ++i)
	{
		sum += m_Colors[i];
	}
	sum /= m_Colors.size();
	return sum;
}
