#include "Line.h"


Lines GetLines(const CvLines& cvp)
{
	Lines res;
	for (auto it = cvp.begin(); it != cvp.end(); ++it)
	{
		Line li;
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			li.push_back(Vector2(it2->x, it2->y));
		}
		res.push_back(li);
	}
	return res;
}


Lines GetLines(const CvLines& cvp, double xOffset, double yOffset)
{
	Lines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(Line());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(Vector2(it2->x + xOffset, it2->y + yOffset));
		}
	}
	return lines;
}

Lines GetLines(const CgalLines& cvp, double xOffset, double yOffset)
{
	Lines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(Line());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(Vector2(it2->hx() + xOffset, it2->hy() + yOffset));
		}
	}
	return lines;
}

Line GetLine(const CvLine& cvp, double xOffset, double yOffset)
{
	Line line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(Vector2(it2->x + xOffset, it2->y + yOffset));
	}
	return line;
}

Line GetLine(const CgalLine& cvp, double xOffset, double yOffset)
{
	Line line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(Vector2(it2->hx() + xOffset, it2->hy() + yOffset));
	}
	return line;
}

CvLines GetCvLines(const Lines& cvp, double xOffset/*=0*/, double yOffset/*=0*/)
{
	CvLines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(CvLine());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(cv::Point(it2->x + xOffset, it2->y + yOffset));
		}
	}
	return lines;
}

CvLines GetCvLines(const CgalLines& cvp, double xOffset/*=0*/,
				   double yOffset/*=0*/)
{
	CvLines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(CvLine());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(cv::Point(it2->hx() + xOffset, it2->hy() + yOffset));
		}
	}
	return lines;
}

CvLine GetCvLine(const Line& cvp, double xOffset/*=0*/, double yOffset/*=0*/)
{
	CvLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(cv::Point(it2->x + xOffset, it2->y + yOffset));
	}
	return line;
}

CvLine GetCvLine(const CgalLine& cvp, double xOffset/*=0*/,
				 double yOffset/*=0*/)
{
	CvLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(cv::Point(it2->hx() + xOffset, it2->hy() + yOffset));
	}
	return line;
}

CgalLines GetCgalLines(const CvLines& cvp, double xOffset/*=0*/,
					   double yOffset/*=0*/)
{
	CgalLines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(CgalLine());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(CgalPoint(it2->x + xOffset, it2->y + yOffset));
		}
	}
	return lines;
}

CgalLines GetCgalLines(const Lines& cvp, double xOffset/*=0*/,
					   double yOffset/*=0*/)
{
	CgalLines lines;
	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(CgalLine());
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(CgalPoint(it2->x + xOffset, it2->y + yOffset));
		}
	}
	return lines;
}

CgalLine GetCgalLine(const CvLine& cvp, double xOffset/*=0*/,
					 double yOffset/*=0*/)
{
	CgalLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(CgalPoint(it2->x + xOffset, it2->y + yOffset));
	}
	return line;
}

CgalLine GetCgalLine(const Line& cvp, double xOffset/*=0*/,
					 double yOffset/*=0*/)
{
	CgalLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(CgalPoint(it2->x + xOffset, it2->y + yOffset));
	}
	return line;
}

Line SmoothingSize5(const Line& cvp, int repeat)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Line cps = cvp;
	Line newcps;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		if (cps.size() <= 5)
		{
			newcps.push_back(cps.front());
			for (int j = 1; j < cps.size() - 1; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) * 0.25;
				newcps.push_back(vec);
			}
			newcps.push_back(cps.back());
			cps = newcps;
		}
		else
		{
			newcps.push_back(cps.front());
			newcps.push_back((cps[0] + cps[1] * 2 + cps[2]) * 0.25);
			for (int j = 2; j < cps.size() - 2; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
						   6.0f;
				newcps.push_back(vec);
			}
			int last = cps.size() - 1;
			newcps.push_back((cps[last] + cps[last - 1] * 2 + cps[last - 2]) * 0.25);
			newcps.push_back(cps.back());
			cps = newcps;
		}
	}
	return cps;
}

Lines SmoothingSize5(const Lines& cvp, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingSize5(nowLine, repeat);
	}
	return cps;
}

Line SmoothingSize3(const Line& cvp, int repeat /*= 1*/)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Line cps = cvp;
	Line newcps;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		newcps.push_back(cps.front());
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) * 0.25;
			newcps.push_back(vec);
		}
		newcps.push_back(cps.back());
		cps = newcps;
	}
	return cps;
}

Lines SmoothingSize3(const Lines& cvp, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingSize3(nowLine, repeat);
	}
	return cps;
}
