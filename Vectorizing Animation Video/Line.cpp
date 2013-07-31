#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Line.h"
#include "math/Quaternion.h"
#include "IFExtenstion.h"

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
			lines.back().push_back(cv::Point(int(it2->x + xOffset), int(it2->y + yOffset)));
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
			lines.back().push_back(cv::Point(int(it2->hx() + xOffset), int(it2->hy() + yOffset)));
		}
	}
	return lines;
}

CvLine GetCvLine(const Line& cvp, double xOffset/*=0*/, double yOffset/*=0*/)
{
	CvLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(cv::Point(int(it2->x + xOffset), int(it2->y + yOffset)));
	}
	return line;
}

CvLine GetCvLine(const CgalLine& cvp, double xOffset/*=0*/,
				 double yOffset/*=0*/)
{
	CvLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(cv::Point(int(it2->hx() + xOffset), int(it2->hy() + yOffset)));
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

Line SmoothingLen5(const Line& cvp, double centroidRadio, int repeat)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Line cps = cvp;
	Line newcps;
	Line centroids;
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
			centroids.clear();
			newcps.push_back(cps.front());
			centroids.push_back((cps[0] + cps[1]  + cps[2]) / 3.0f);
			newcps.push_back((cps[0] + cps[1] * 2 + cps[2]) * 0.25);
			for (int j = 2; j < cps.size() - 2; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
						   6.0f;
				newcps.push_back(vec);
				centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
									 cps[j - 2]) / 5.0f);
			}
			int last = (int)cps.size() - 1;
			newcps.push_back((cps[last] + cps[last - 1] * 2 + cps[last - 2]) * 0.25);
			centroids.push_back((cps[last] + cps[last - 1]  + cps[last - 2]) / 3.0f);
			newcps.push_back(cps.back());
			cps = newcps;
			// move centroid
			newcps.clear();
			newcps.push_back(cps.front());
			Vector2 cert = (cps[0] + cps[1]  + cps[2]) / 3.0f;
			cert = centroids[0] - cert;
			newcps.push_back(cps[1] + cert);
			for (int j = 2; j < (int)cps.size() - 2; j ++)
			{
				Vector2 nowCentroid = (cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
									   cps[j - 2]) / 5.0f;
				nowCentroid = centroids[j - 1] - nowCentroid;
				newcps.push_back(cps[j] + nowCentroid * centroidRadio);
			}
			cert = (cps[last] + cps[last - 1]  + cps[last - 2]) / 3.0f;
			cert = centroids[last - 2] - cert;
			newcps.push_back(cps[last - 1] + cert);
			newcps.push_back(cps.back());
			cps = newcps;
		}
	}
	return cps;
}

Lines SmoothingLen5(const Lines& cvp, double centroidRadio, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingLen5(nowLine, centroidRadio, repeat);
	}
	return cps;
}

Line SmoothingLen3(const Line& cvp, double centroidRadio, int repeat /*= 1*/)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Line cps = cvp;
	Line newcps;
	Line centroids;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		centroids.clear();
		newcps.clear();
		newcps.push_back(cps.front());
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) * 0.25;
			newcps.push_back(vec);
			centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1]) / 3.0f);
		}
		newcps.push_back(cps.back());
		cps = newcps;
		// move centroid
		newcps.clear();
		newcps.push_back(cps.front());
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			Vector2 nowCentroid = (cps[j] + cps[j + 1] + cps[j - 1]) / 3.0f;
			nowCentroid = centroids[j - 1] - nowCentroid;
			newcps.push_back(cps[j] + nowCentroid * centroidRadio);
		}
		newcps.push_back(cps.back());
		cps = newcps;
	}
	return cps;
}

Lines SmoothingLen3(const Lines& cvp, double centroidRadio, int repeat /*= 1*/)
{
	Lines cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp[i];
		cps[i] = SmoothingLen3(nowLine, centroidRadio, repeat);
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
		int last = (int)cvp.size() - 1;
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
			int j = i - range;
			int jend = i + range;
			if (j < 0)
			{
				j = 0;
			}
			if (jend >= cvp.size())
			{
				jend = (int)cvp.size() - 1;
			}
			for (;j < jend; ++j)
			{
				if (cvp[j].x > 0.1)
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
				if (cvp[j].x > 0.1)
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

Line SmoothingHas0Len5(const Line& cvp, double centroidRadio, int repeat)
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
			newcps.push_back(cps.front() * 0.8);
			newcps.push_back((cps[0] + cps[1] * 2 + cps[2]) * 0.25);
			for (int j = 2; j < (int)cps.size() - 2; j ++)
			{
				if (cps[j].x > 0)
				{
					int zero = 0;
					for (int k = j - 2; k <= j + 2; ++k)
					{
						if (cps[k].x < 0.01)
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
			int last = (int)cps.size() - 1;
			newcps.push_back((cps[last] + cps[last - 1] * 2 + cps[last - 2]) * 0.25);
			newcps.push_back(cps.back() * 0.8);
			cps = newcps;
		}
	}
	return cps;
}

Lines SmoothingHas0Len5(const Lines& cvp, double centroidRadio, int num)
{
	Lines ans(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& aa = cvp.at(i);
		ans[i] = SmoothingHas0Len5(aa, centroidRadio, num);
	}
	return ans;
}

Line SmoothingHas0Len3(const Line& cvp, double centroidRadio, int repeat)
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
			newcps.push_back(cps.front() * 0.8);
			for (int j = 2; j < cps.size() - 2; j ++)
			{
				if (cps[j].x > 0)
				{
					int zero = 0;
					for (int k = j - 1; k <= j + 1; ++k)
					{
						if (cps[k].x < 0.01)
						{
							zero++;
						}
					}
					auto vec = (cps[j] * (2 + zero) + cps[j + 1] + cps[j - 1]) / 4.0f;
					newcps.push_back(vec);
				}
				else
				{
					newcps.push_back(Vector2());
				}
			}
			newcps.push_back(cps.back() * 0.8);
			cps = newcps;
		}
	}
	return cps;
}

Lines SmoothingHas0Len3(const Lines& cvp, double centroidRadio, int num)
{
	Lines ans(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& aa = cvp.at(i);
		ans[i] = SmoothingHas0Len3(aa, centroidRadio, num);
	}
	return ans;
}

Lines SplitStraightLine(const Line& line,  double angle)
{
	Lines ans;
	double_vector angles = Accumulation(Curvature(ComputeAngle(line)));
	double threshold = 0;
	assert(angles.size() == line.size());
	int lastidx = 0;
	double nowAngle = 0;
	for (int i = 1; i < angles.size(); ++i)
	{
		if (abs(angles[i] - threshold) > angle)
		{
			ans.push_back(Line());
			Line& nowLine = ans.back();
			nowLine.insert(nowLine.begin(), line.begin() + lastidx, line.begin() + i);
			lastidx = i;
			threshold = angles[i];
		}
	}
	return ans;
}

Lines SplitStraightLine(const Lines& cvp, double angle)
{
	Lines ans;
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& aa = cvp.at(i);
		Lines newLines = SplitStraightLine(aa, angle);
		ans.insert(ans.end(), newLines.begin(), newLines.end());
	}
	return ans;
}

LinesPair SplitStraightLineAndWidth(const Line& line, const Line& width,
									double angle)
{
	LinesPair ans;
	Lines& ans1 = ans.first;
	Lines& ans2 = ans.second;
	double_vector angles = Accumulation(Curvature(ComputeAngle(line)));
	double threshold = 0;
	assert(angles.size() == line.size());
	int lastidx = 0;
	double nowAngle = 0;
	for (int i = 1; i < angles.size(); ++i)
	{
		if (abs(angles[i] - threshold) > angle)
		{
			ans1.push_back(Line());
			ans2.push_back(Line());
			Line& nowLine = ans1.back();
			nowLine.insert(nowLine.begin(), line.begin() + lastidx, line.begin() + i + 1);
			Line& nowWidth = ans2.back();
			nowWidth.insert(nowWidth.begin(), width.begin() + lastidx,
							width.begin() + i + 1);
			lastidx = i;
			threshold = angles[i];
		}
	}
	ans1.push_back(Line());
	ans2.push_back(Line());
	Line& nowLine = ans1.back();
	Line& nowWidth = ans2.back();
	nowLine.insert(nowLine.begin(), line.begin() + lastidx, line.end());
	nowWidth.insert(nowWidth.begin(), width.begin() + lastidx, width.end());
	return ans;
}

LinesPair SplitStraightLineAndWidth(const Lines& cvp, const Lines& width,
									double angle)
{
	LinesPair ans;
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& aa = cvp.at(i);
		const Line& bb = width.at(i);
		LinesPair newLines = SplitStraightLineAndWidth(aa, bb, angle);
		ans.first.insert(ans.first.end(), newLines.first.begin(), newLines.first.end());
		ans.second.insert(ans.second.end(), newLines.second.begin(),
						  newLines.second.end());
	}
	return ans;
}


double_vector ComputeAngle(const Line& line)
{
	double_vector ans(line.size(), 0);
	for (int i = 1; i < line.size(); i++)
	{
		Vector2 vec = line[i] - line[i - 1];
		ans[i] = atan2(vec.x, vec.y) / M_PI * 180;
	}
	ans[0] = ans[1];
	return ans;
}

Line GetControlPoint(const Line& line, double angle)
{
	Line ans;
	double_vector angles = Accumulation(Curvature(ComputeAngle(line)));
	double threshold = 0;
	assert(angles.size() == line.size());
	ans.push_back(line.front());
	for (int i = 1; i < angles.size() - 1; ++i)
	{
		if (abs(angles[i] - threshold) > angle)
		{
			ans.push_back(line[i]);
			threshold = angles[i];
		}
	}
	ans.push_back(line.back());
	return ans;
}

Endpoints GetEndpoints(const Lines& cvp)
{
	Endpoints ans;
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp.at(i);
		Vector2 vec = nowLine[0] - nowLine[2];
		double angle = atan2(vec.x, vec.y) / M_PI * 180;
		ans.push_back(Endpoint(nowLine.front(), i, 0, angle));
		int last = (int)nowLine.size() - 1;
		vec = nowLine[last] - nowLine[last - 2];
		angle = atan2(vec.x, vec.y) / M_PI * 180;
		ans.push_back(Endpoint(nowLine.back(), i, last, angle));
	}
	return ans;
}

bool LessEndpointX(const Endpoint& a, const Endpoint& b)
{
	return a.pos.x < b.pos.x;
}

bool GreaterEndpointX(const Endpoint& a, const Endpoint& b)
{
	return a.pos.x > b.pos.x;
}

bool LessEndpointY(const Endpoint& a, const Endpoint& b)
{
	return a.pos.y < b.pos.y;
}

bool GreaterEndpointY(const Endpoint& a, const Endpoint& b)
{
	return a.pos.y > b.pos.y;
}


void SortEndpointsX(Endpoints& cvp)
{
	std::sort(cvp.begin(), cvp.end(), LessEndpointX);
}

Endpoints FindNearEndpoints(const Endpoints& cvp, const Vector2& pos,
							double distance)
{
	distance *= distance;
	Endpoints ans;
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Endpoint& nowPoint = cvp.at(i);
		double tmpDis = nowPoint.pos.squaredDistance(pos);
		if (tmpDis < distance && tmpDis > 0.001)
		{
			if (ans.empty())
			{
				ans.push_back(nowPoint);
			}
			// avoid same line
			else if (ans.back().idx1 == nowPoint.idx1)
			{
				if (tmpDis < ans.back().pos.squaredDistance(pos))
				{
					ans.erase(ans.end() - 1);
					ans.push_back(nowPoint);
				}
			}
			else
			{
				ans.push_back(nowPoint);
			}
		}
	}
	return ans;
}

Endpoints FindNearSortedXEndpoints(const Endpoints& cvp, const Vector2& pos,
								   double distance)
{
	Endpoints ans;
	return ans;
}

bool CheckEndpointsSimilarity(const Endpoint& lhs, const Endpoint& rhs,
							  double angle)
{
	Vector2 vec = lhs.pos - rhs.pos;
	double linkAngle = atan2(vec.x, vec.y) / M_PI * 180;
	double refAngleL = lhs.angle + 180;
	double refAngleR = rhs.angle + 180;
	if (refAngleL >= 360)
	{
		refAngleL -= 360;
	}
	if (refAngleR >= 360)
	{
		refAngleR -= 360;
	}
	if (abs(lhs.angle - linkAngle) < angle || abs(refAngleL - linkAngle) < angle)
	{
		if (abs(rhs.angle - linkAngle) < angle || abs(refAngleR - linkAngle) < angle)
		{
			return true;
		}
	}
	return false;
}

void ConnectSimilarLines(Lines& pos, Lines& width, double angle)
{
	Endpoints eps = GetEndpoints(pos);
	for (int i = 0; i < eps.size(); ++i)
	{
		Endpoints nearEps = FindNearEndpoints(eps, eps[i].pos, 10);
		for (int j = 0; j < nearEps.size(); ++j)
		{
			if (nearEps[j] != eps[i] && CheckEndpointsSimilarity(nearEps[j], eps[i], angle))
			{
				if (eps[i].idx2 > 0)
				{
					Line& linkLine = pos[eps[i].idx1];
					Line& beLink = pos[nearEps[j].idx1];
					Line& w1 = width[eps[i].idx1];
					Line& w2 = width[nearEps[j].idx1];
					if (linkLine.empty())
					{
						continue;
					}
					if (nearEps[j].idx2 == 0)
					{
						linkLine.insert(linkLine.end(), beLink.begin(), beLink.end());
						w1.insert(w1.end(), w2.begin(), w2.end());
					}
					else
					{
						linkLine.insert(linkLine.end(), beLink.rbegin(), beLink.rend());
						w1.insert(w1.end(), w2.rbegin(), w2.rend());
					}
					beLink.clear();
					w2.clear();
					// change Endpoint info
					eps[i].idx2 = (int)linkLine.size() - 1;
					eps[i].pos = linkLine.back();
					Vector2 vec = linkLine.back() - linkLine[linkLine.size() - 3];
					eps[i].angle = atan2(vec.x, vec.y);
					// remove Endpoint info
					const int removeId = nearEps[j].idx1;
					for (int k = 0; k < eps.size(); ++k)
					{
						if (eps[k].idx1 == removeId)
						{
							eps[k].idx1 = -1;
							eps[k].pos.x = -100;
							eps[k].pos.y = -100;
						}
					}
				}
				else if (nearEps[j].idx2 > 0)
				{
					Line& linkLine = pos[nearEps[j].idx1];
					Line& beLink = pos[eps[i].idx1];
					Line& w1 = width[nearEps[j].idx1];
					Line& w2 = width[eps[i].idx1];
					if (linkLine.empty())
					{
						continue;
					}
					if (eps[j].idx2 == 0)
					{
						linkLine.insert(linkLine.end(), beLink.begin(), beLink.end());
						w1.insert(w1.end(), w2.begin(), w2.end());
					}
					else
					{
						linkLine.insert(linkLine.end(), beLink.rbegin(), beLink.rend());
						w1.insert(w1.end(), w2.rbegin(), w2.rend());
					}
					beLink.clear();
					w2.clear();
					// change Endpoint info
					for (int k = 0; k < eps.size(); ++k)
					{
						if (eps[k] == nearEps[j])
						{
							eps[k].idx2 = (int)linkLine.size() - 1;
							eps[k].pos = linkLine.back();
							Vector2 vec = linkLine.back() - linkLine[linkLine.size() - 3];
							eps[i].angle = atan2(vec.x, vec.y);
							break;
						}
					}
					// remove Endpoint info
					const int removeId = eps[i].idx1;
					for (int k = 0; k < eps.size(); ++k)
					{
						if (eps[k].idx1 == removeId)
						{
							eps[k].idx1 = -1;
							eps[k].pos.x = -100;
							eps[k].pos.y = -100;
						}
					}
				}
				else if (eps[i].idx2 == 0)
				{
					Line& linkLine = pos[eps[i].idx1];
					Line& beLink = pos[nearEps[j].idx1];
					Line& w1 = width[eps[i].idx1];
					Line& w2 = width[nearEps[j].idx1];
					if (linkLine.empty())
					{
						continue;
					}
					if (nearEps[j].idx2 == 0)
					{
						linkLine.insert(linkLine.begin(), beLink.rbegin(), beLink.rend());
						w1.insert(w1.begin(), w2.rbegin(), w2.rend());
					}
					else
					{
						linkLine.insert(linkLine.begin(), beLink.begin(), beLink.end());
						w1.insert(w1.begin(), w2.begin(), w2.end());
					}
					beLink.clear();
					w2.clear();
					// change Endpoint info
					for (int k = 0; k < eps.size(); ++k)
					{
						if (eps[k].idx1 == eps[i].idx1)
						{
							if (eps[k].idx2 > 0)
							{
								eps[k].idx2 = (int)linkLine.size() - 1;
								break;
							}
						}
					}
					eps[i].pos = linkLine.front();
					Vector2 vec = linkLine.front() - linkLine[2];
					eps[i].angle = atan2(vec.x, vec.y);
					// remove Endpoint info
					const int removeId = nearEps[j].idx1;
					for (int k = 0; k < eps.size(); ++k)
					{
						if (eps[k].idx1 == removeId)
						{
							eps[k].idx1 = -1;
							eps[k].pos.x = -100;
							eps[k].pos.y = -100;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < pos.size(); ++i)
	{
		if (pos[i].empty())
		{
			pos.erase(pos.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < width.size(); ++i)
	{
		if (width[i].empty())
		{
			width.erase(width.begin() + i);
			i--;
		}
	}
}

double GetLineWidthPercent(const Line& cvp)
{
	double n = 0;
	for (int i = 0; i < cvp.size(); ++i)
	{
		if (cvp[i].x > 0.1)
		{
			n += 1;
		}
		else if (cvp[i].y > 0.1)
		{
			n += 1;
		}
	}
	return n / cvp.size();
}

void ClearLineWidthByPercent(Lines& widths, double angle)
{
	for (int i = 0; i < widths.size(); ++i)
	{
		Line& nowWidths = widths.at(i);
		double p = GetLineWidthPercent(nowWidths);
		if (p < angle)
		{
			nowWidths = Line(nowWidths.size());
		}
		else
		{
			nowWidths = FixLineWidths(nowWidths, 200);
		}
	}
}
