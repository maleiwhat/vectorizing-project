#include "Line.h"
#include "math/Quaternion.h"


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

Lines GetLines(const Lines& cvp)
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

Line GetLine(const Line& cvp, double xOffset, double yOffset)
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

Line SmoothingLen5(const Line& cvp, int repeat)
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

Lines SmoothingLen5(const Lines& cvp, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingLen5(nowLine, repeat);
	}
	return cps;
}

Line SmoothingLen3(const Line& cvp, int repeat /*= 1*/)
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

Lines SmoothingLen3(const Lines& cvp, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingLen3(nowLine, repeat);
	}
	return cps;
}

Line GetNormalsLen3(const Line& cvp)
{
	Line normals;
	if (cvp.size() > 3)
	{
		normals.push_back(Quaternion::GetRotation(cvp[2] - cvp[0], -90));
		for (int i = 1; i < cvp.size() - 1 ; ++i)
		{
			normals.push_back(Quaternion::GetRotation(cvp[i + 1] - cvp[i - 1],
							  -90));
		}
		int last = cvp.size() - 1;
		normals.push_back(Quaternion::GetRotation(cvp[last] - cvp[last - 2], -90));
		for (int i = 0; i < normals.size() ; ++i)
		{
			normals[i].normalise();
		}
	}
	else
	{
		normals = GetNormalsLen2(cvp);
	}
	return normals;
}

Line GetNormalsLen2(const Line& cvp)
{
	Line normals;
	normals.push_back(Quaternion::GetRotation(cvp[1] - cvp[0], -90));
	for (int i = 1; i < cvp.size(); ++i)
	{
		normals.push_back(Quaternion::GetRotation(cvp[i] - cvp[i - 1],
						  -90));
	}
	for (int i = 0; i < normals.size() ; ++i)
	{
		normals[i].normalise();
	}
	return normals;
}

Lines GetNormalsLen2(const Lines& cvp)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = GetNormalsLen2(nowLine);
	}
	return cps;
}

Lines GetNormalsLen3(const Lines& cvp)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = GetNormalsLen3(nowLine);
	}
	return cps;
}

Line LineAdd(const Line& aLine, const Line& bLine)
{
	Line ans(aLine.size());
	for (int i = 0; i < aLine.size(); ++i)
	{
		const Vector2& aa = aLine.at(i);
		const Vector2& bb = bLine.at(i);
		ans[i] = aa + bb;
	}
	return ans;
}

Lines LinesAdd(const Lines& aLine, const Lines& bLine)
{
	Lines ans(aLine.size());
	for (int i = 0; i < aLine.size(); ++i)
	{
		const Line& aa = aLine.at(i);
		const Line& bb = bLine.at(i);
		ans[i] = LineAdd(aa, bb);
	}
	return ans;
}

Line LineSub(const Line& aLine, const Line& bLine)
{
	Line ans(aLine.size());
	for (int i = 0; i < aLine.size(); ++i)
	{
		const Vector2& aa = aLine.at(i);
		const Vector2& bb = bLine.at(i);
		ans[i] = aa - bb;
	}
	return ans;
}

Lines LinesSub(const Lines& aLine, const Lines& bLine)
{
	Lines ans(aLine.size());
	for (int i = 0; i < aLine.size(); ++i)
	{
		const Line& aa = aLine.at(i);
		const Line& bb = bLine.at(i);
		ans[i] = LineSub(aa, bb);
	}
	return ans;
}

// 左右端點有黑線範圍的話就把中間空的連起來
Line FixLineWidths(const Line& cvp, int range)
{
	Line cps = cvp;
	for (int i = 0; i < cvp.size(); ++i)
	{
		if (0 == cvp[i].x)
		{
			Vector2 setValue;
			int finds = 0;
			// 有左端點
			bool hasLeft = false;
			bool hasRight = false;
			for (int j = i - range; (j < i + range) && (j < cvp.size()); ++j)
			{
				if (cvp[j].x > 0)
				{
					finds++;
					setValue += cvp[j];
					if (j < i)
					{
						hasLeft = true;
					}
					else if (j > i)
					{
						hasRight = true;
					}
				}
			}
			if (hasLeft && hasRight)
			{
				setValue /= finds;
				cps[i] = setValue;
			}
		}
	}
	return cps;
}

Lines FixLineWidths(const Lines& widths, int range)
{
	Lines ans(widths.size());
	for (int i = 0; i < widths.size(); ++i)
	{
		const Line& aa = widths.at(i);
		ans[i] = FixLineWidths(aa, range);
	}
	return ans;
}

Line CleanOrphanedLineWidths(const Line& cvp, int num)
{
	Line cps = cvp;
	for (int i = 0; i < cvp.size(); ++i)
	{
		if (cvp[i].x > 0)
		{
			int hasWidth = 1;
			for (int j = i + 1; j < cvp.size(); ++j)
			{
				if (cvp[j].x > 0)
				{
					hasWidth++;
				}
				else
				{
					if (hasWidth < num)
					{
						for (int k = i; k < i + hasWidth; ++k)
						{
							cps[k].x = 0;
							cps[k].y = 0;
						}
					}
					i = j;
					break;
				}
			}
		}
	}
	return cps;
}

Lines CleanOrphanedLineWidths(const Lines& widths, int num)
{
	Lines ans(widths.size());
	for (int i = 0; i < widths.size(); ++i)
	{
		const Line& aa = widths.at(i);
		ans[i] = CleanOrphanedLineWidths(aa, num);
	}
	return ans;
}

Line SmoothingHas0Len5(const Line& cvp, int repeat)
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
			newcps.push_back(cps.front()*0.8);
			newcps.push_back((cps[0] + cps[1] * 2 + cps[2]) * 0.25);
			for (int j = 2; j < cps.size() - 2; j ++)
			{
				if (cps[j].x > 0)
				{
					int zero = 0;
					for (int k = j - 2; k <= j + 2; ++k)
					{
						if (cps[j].x < 0.01)
						{
							zero++;
						}
					}
					auto vec = (cps[j] * (2 + zero) + cps[j + 1] + cps[j - 1] + cps[j + 2] +
								cps[j - 2]) / 6.0f;
					newcps.push_back(vec);
				}
				else
				{
					newcps.push_back(Vector2());
				}
			}
			int last = cps.size() - 1;
			newcps.push_back((cps[last] + cps[last - 1] * 2 + cps[last - 2]) * 0.25);
			newcps.push_back(cps.back()*0.8);
			cps = newcps;
		}
	}
	return cps;
}

Lines SmoothingHas0Len5(const Lines& cvp, int num)
{
	Lines ans(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& aa = cvp.at(i);
		ans[i] = SmoothingHas0Len5(aa, num);
	}
	return ans;
}
