#pragma once
#include <vector>
// �r���j�p�B�����j�p��
struct xSize
{
	xSize();
	xSize(int _w, int _h);
	int w, h;
};
// �ƹ��y�СBø�Ϯy�Х�
struct xVec2
{
	xVec2();
	xVec2(int _x, int _y);
	int x, y;
};
typedef std::vector<xVec2> xVec2s;
// �e�x�ΡB�y�z��t�ťժ��ץ�
struct xRect
{
	xRect();
	xRect(int _x, int _y, int _w, int _h);
	union
	{
		struct // for DrawRect
		{
			int x, y, w, h;
		};
		struct // for MarginSize
		{
			int left, right, top, bottom;
		};
	};
	// �P�_�y�ЬO�_�b�x�Τ�
	bool Collision(int _x, int _y);
};
// �y�z�C��
struct Color
{
	Color();
	Color(unsigned char _r, unsigned char _g, unsigned char _b);
	unsigned char r, g, b;
};
// �����u�ʱ�
struct VerticalScroll
{
	static const int MIN_SCROLL_WIDTH = 5;
	void Init(int x, int y, int w, int h, int maxv, int minv, int v);
	// w is mount of all lines, vw is mount of lines can view
	void AutoFitScrollWidth(int w, int vw);
	void UpdateValue(int v);
	void MouseDown(int v);
	int MouseMove(int v);
	void StepUp();
	void StepDown();
	// �u�ʮؽd��
	xRect m_Frame;
	// �u�ʱ����d��
	xRect m_Scroll;
	// �u�ʱ�������
	int m_ScrollWidth;
	// �N���̤j��
	int m_MaxValue;
	// �N���̤p��
	int m_MinValue;
	// �N����
	int m_Value;
	// �ƹ����U��������
	int m_MouseDownValue;
	// �ƹ����U������Y
	int m_MouseY;
	// �ݪ��쪺�d��
	int m_ViewWidth;
};

// �����u�ʱ�
struct HorizontalScroll
{
	static const int MIN_SCROLL_WIDTH = 5;
	void Init(int x, int y, int w, int h, int maxv, int minv, int v);
	// w is mount of all lines, vw is mount of lines can view
	void AutoFitScrollWidth(int w, int vw);
	void UpdateValue(int v);
	void MouseDown(int v);
	int MouseMove(int v);
	void StepUp();
	void StepDown();
	// �u�ʮؽd��
	xRect m_Frame;
	// �u�ʱ����d��
	xRect m_Scroll;
	// �u�ʱ�������
	int m_ScrollWidth;
	// �N���̤j��
	int m_MaxValue;
	// �N���̤p��
	int m_MinValue;
	// �N����
	int m_Value;
	// �ƹ����U��������
	int m_MouseDownValue;
	// �ƹ����U������Y
	int m_MouseY;
	// �ݪ��쪺�d��
	int m_ViewWidth;
};

