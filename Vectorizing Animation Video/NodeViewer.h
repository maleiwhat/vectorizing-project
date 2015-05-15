#pragma once
#include <string>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "General.h"
#include "DrawNode.h"

class NodeViewer
{
public:
	NodeViewer(HWND hwnd);
	~NodeViewer(void);
	// 得到整個EditorConcept的大小
	xSize    GetSize();
	// 設定大小，設定超過視窗大小會畫超過
	void    SetSize(int w, int h);
	void	DrawRect(const xRect& area, const Color& c);
	void	DrawLeftString(int x, int y, const std::wstring& s);
	void	DrawScroll();
	void	SetFontSize(const xSize& s);
	xSize	GetFontSize();

	void	MouseLDown(int x, int y);
	void	MouseMove(int x, int y);
	void	MouseLUp(int x, int y);

	void	NeedRender();
	void	Render();

	void	AddLink(std::vector<int>& v1, std::vector<int>& v2);
	void	AddNodeLine(int n);
	void	AddNodeLink(int n1, int n2);

	void	StepUp();
	void	StepDown();
protected:
	void	InterRenderStart();
	void	InterRenderEnd();
	void	InterRenderNode();
	void	InterRenderLine();
	void	InterRenderBackGround();
	

	DrawNodes	m_DrawNodes;


	HFONT	m_hfont;
	HWND	m_hwnd;
	HDC		m_hdc;
	PAINTSTRUCT	m_ps;
	HDC		m_hdcBackBuffer;
	HBITMAP m_hbmBackBuffer;
	RECT	m_rcSize;

	xSize   m_Size;
	xRect   m_MarginSize;
	xVec2	m_Offset;
	Color   m_MarginColor;
	Color   m_BackGroundColor;
	xSize   m_FontSize;
	VerticalScroll m_VerticalScroll;
	HorizontalScroll m_HorizontalScroll;
	HorizontalScroll m_HScroll_Frames;
	xVec2    m_LDownPosition;
	xVec2    m_LUpPosition;

	xVec2s	m_Pair1;
	xVec2s	m_Pair2;

public:
	// frame id
	int		m_Focus1;
	// region id
	int		m_Focus2;
	// focus frame
	int		m_Focus3;
	int		m_HScrollF_x;
	int		m_maxElement;
	int		m_maxFrame;
	int		m_Lines;
	bool    m_LDownVScroll;
	bool    m_LDownHScroll;
	bool    m_LDownHScrollF;
};

