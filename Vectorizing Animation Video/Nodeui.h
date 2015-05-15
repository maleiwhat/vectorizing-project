#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <vector>
#include <string>
#include <windows.h>
#include "NodeViewer.h"

typedef std::vector<cv::Mat> Mats;
typedef std::vector<std::string> strings;
typedef std::vector<HWND> HWNDs;
typedef std::vector<char> chars;

class Nodeui
{
public:
	HWND  m_main;
	HWNDs m_hwnd_checks;
	Mats  m_mats;
	cv::Mat m_sizeimg;
	chars m_ischecks;
	strings m_names;
	int m_num_check;
	Nodeui(void);
	~Nodeui(void);
	void Init();
	void Render();
	int w, h;
	NodeViewer* m_viewer;
};

extern Nodeui g_Nodeui;