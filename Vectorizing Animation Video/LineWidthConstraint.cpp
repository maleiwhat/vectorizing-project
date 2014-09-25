#include "LineWidthConstraint.h"


LineWidthConstraint::LineWidthConstraint(void)
	: m_NeedBuildModel(true)
{
}


LineWidthConstraint::~LineWidthConstraint(void)
{
}

void LineWidthConstraint::AddPoint(double x, double y, double w1, double w2)
{
	m_NeedBuildModel = true;
	m_Pos.push_back(Vector2(x, y));
	m_Widths1.push_back(w1);
	m_Widths2.push_back(w2);
}

void LineWidthConstraint::Clear()
{
	m_Pos.clear();
}

void LineWidthConstraint::BuildModel()
{
	m_NeedBuildModel = false;
	const int LEN = m_Widths1.size();
	const int DIM = 3;
	MatrixXd A(LEN, DIM), At, Ata;
	VectorXd b1(LEN), Atb1;
	VectorXd b2(LEN), Atb2;
	int c = 0;
	for(int i = 0; i < m_Widths1.size(); ++i)
	{
		if(i == 0)
		{
			m_len = 0;
		}
		else
		{
			m_len += m_Pos[i].distance(m_Pos[i - 1]);
		}
		b1.coeffRef(c) = m_Widths1[i];
		b2.coeffRef(c) = m_Widths2[i];
		A.coeffRef(c, 0) = 1;
		A.coeffRef(c, 1) = m_len;
		A.coeffRef(c, 2) = m_len * m_len;
		c++;
	}
	At = A.transpose();
	Ata = At * A;
	Atb1 = At * b1;
	Atb2 = At * b2;
	Eigen::PartialPivLU<MatrixXd> Atalu = Ata.lu();
	x1 = Atalu.solve(Atb1);
	x2 = Atalu.solve(Atb2);
}


