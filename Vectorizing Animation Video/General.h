#pragma once
#include <vector>
// 字的大小、視窗大小用
struct xSize
{
	xSize();
	xSize(int _w, int _h);
	int w, h;
};
// 滑鼠座標、繪圖座標用
struct xVec2
{
	xVec2();
	xVec2(int _x, int _y);
	int x, y;
};
typedef std::vector<xVec2> xVec2s;
// 畫矩形、描述邊緣空白長度用
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
	// 判斷座標是否在矩形內
	bool Collision(int _x, int _y);
};
// 描述顏色
struct Color
{
	Color();
	Color(unsigned char _r, unsigned char _g, unsigned char _b);
	unsigned char r, g, b;
};
// 直的滾動條
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
	// 滾動框範圍
	xRect m_Frame;
	// 滾動條的範圍
	xRect m_Scroll;
	// 滾動條的長度
	int m_ScrollWidth;
	// 代表的最大值
	int m_MaxValue;
	// 代表的最小值
	int m_MinValue;
	// 代表的值
	int m_Value;
	// 滑鼠按下瞬間的值
	int m_MouseDownValue;
	// 滑鼠按下瞬間的Y
	int m_MouseY;
	// 看的到的範圍
	int m_ViewWidth;
};

// 直的滾動條
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
	// 滾動框範圍
	xRect m_Frame;
	// 滾動條的範圍
	xRect m_Scroll;
	// 滾動條的長度
	int m_ScrollWidth;
	// 代表的最大值
	int m_MaxValue;
	// 代表的最小值
	int m_MinValue;
	// 代表的值
	int m_Value;
	// 滑鼠按下瞬間的值
	int m_MouseDownValue;
	// 滑鼠按下瞬間的Y
	int m_MouseY;
	// 看的到的範圍
	int m_ViewWidth;
};

