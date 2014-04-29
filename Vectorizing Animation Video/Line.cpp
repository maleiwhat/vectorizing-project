#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Line.h"
#include "math/Quaternion.h"
#include "IFExtenstion.h"
#include "ColorConstraintMedian.h"

#ifdef USE_CGAL
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


Line GetLine(const CgalLine& cvp, double xOffset, double yOffset)
{
	Line line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(Vector2(it2->hx() + xOffset, it2->hy() + yOffset));
	}
	return line;
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
			lines.back().push_back(cv::Point(int(it2->hx() + xOffset),
											 int(it2->hy() + yOffset)));
		}
	}
	return lines;
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
#endif // USE_CGAL

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

Lines GetLines(const Lines& cvp, double xOffset, double yOffset)
{
	Lines res;
	for (auto it = cvp.begin(); it != cvp.end(); ++it)
	{
		Line li;
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			li.push_back(Vector2(it2->x + xOffset, it2->y + yOffset));
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

CvLine GetCvLine(const Line& cvp, double xOffset/*=0*/, double yOffset/*=0*/)
{
	CvLine line;
	for (auto it2 = cvp.begin(); it2 != cvp.end(); ++it2)
	{
		line.push_back(cv::Point(int(it2->x + xOffset), int(it2->y + yOffset)));
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

Vector3s SmoothingLen5(const Vector3s& cvp, double centroidRadio /*= 1.0*/, int repeat /*= 1*/)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Vector3s cps = cvp;
	Vector3s newcps;
	Vector3s centroids;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		if (cps.size() <= 6)
		{
			newcps.push_back(cps.front());
			for (int j = 1; j < cps.size() - 1; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1]) * 0.25;
				newcps.push_back(vec);
			}
			newcps.push_back(cps.back());
			if (cps.size() == newcps.size())
			{
				cps = newcps;
			}
		}
		else
		{
			centroids.clear();
			centroids.push_back((cps[0] + cps[1] + cps[2]) * 0.33);
			newcps.push_back(cps[0]);
			newcps.push_back((cps[0] + cps[1] + cps[2]) * 0.33);
//          for (int j = 0; j < 5; j ++)
//          {
//              newcps.push_back(cps[j]);
//              centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
//                  cps[j - 2]) / 5.0f);
//          }
			for (int j = 2; j < cps.size() - 2; j ++)
			{
				Vector3 vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) * 0.166;
				newcps.push_back(vec);
				centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
									 cps[j - 2]) / 5.0f);
			}
//          for (int j = cps.size() - 5; j < cps.size(); j ++)
//          {
//              newcps.push_back(cps[j]);
//              centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
//                  cps[j - 2]) / 5.0f);
//          }
			int last = (int)cps.size() - 1;
			newcps.push_back((cps[last] + cps[last - 1] * 2 + cps[last - 2]) * 0.25);
			centroids.push_back((cps[last] + cps[last - 1]  + cps[last - 2]) * 0.33);
			newcps.push_back(cps.back());
			if (cps.size() == newcps.size())
			{
				cps = newcps;
			}
			// move centroid
			newcps.clear();
			newcps.push_back(cps.front());
			Vector3 cert = (cps[0] + cps[1] + cps[2]) / 3.0f;
			cert = centroids[0] - cert;
			newcps.push_back(cps[1] + cert * centroidRadio);
			for (int j = 2; j < (int)cps.size() - 2; j ++)
			{
				Vector3 nowCentroid = (cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
									   cps[j - 2]) / 5.0f;
				nowCentroid = centroids[j - 1] - nowCentroid;
				newcps.push_back(cps[j] + nowCentroid * centroidRadio);
			}
			cert = (cps[last] + cps[last - 1]  + cps[last - 2]) / 3.0f;
			cert = centroids[last - 2] - cert;
			newcps.push_back(cps[last - 1] + cert * centroidRadio);
			newcps.push_back(cps.back());
			if (cps.size() == newcps.size())
			{
				cps = newcps;
			}
		}
	}
	return cps;
}

Vector3s2d SmoothingLen5(const Vector3s2d& cvp, double centroidRadio /*= 1.0*/, int repeat /*= 1*/)
{
	Vector3s2d cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Vector3s& nowLine = cvp[i];
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
	if (cvp.size() < 2)
	{
		return Line(cvp.size());
	}
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
			for (; j < jend; ++j)
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


ints FixIndexs(const ints& cvp, int range)
{
	ints cps = cvp;
	for (int i = 0; i < cvp.size(); ++i)
	{
		if (cvp[i] < 1)
		{
			int setValueX = -1;
			int j = 1;
			for (; j < range; ++j)
			{
				int left = i - j;
				int right = i + j;
				if (left > 0 && cvp[left] > 0)
				{
					setValueX = cvp[left];
					break;
				}
				if (right < cvp.size() && cvp[right] > 0)
				{
					setValueX = cvp[right];
					break;
				}
			}
			if (setValueX != -1)
			{
				cps[i] = setValueX;
			}
		}
	}
	return cps;
}

ints2d FixIndexs(const ints2d& widths, int range)
{
	ints2d ans(widths.size());
	for (int i = 0; i < widths.size(); ++i)
	{
		const ints& aa = widths.at(i);
		ans[i] = FixIndexs(aa, range);
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

Vector3s SmoothingHas0Len5(const Vector3s& cvp, double centroidRadio /*= 1.0*/, int repeat /*= 1*/)
{
	if (cvp.size() <= 2)
	{
		return cvp;
	}
	Vector3s cps = cvp;
	Vector3s newcps;
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
				if (cps[j].x > 0 || cps[j].y > 0 || cps[j].z > 0)
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
					newcps.push_back(Vector3());
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

Vector3s2d SmoothingHas0Len5(const Vector3s2d& cvp, double centroidRadio /*= 1.0*/,
							 int repeat /*= 1*/)
{
	Vector3s2d ans(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Vector3s& aa = cvp.at(i);
		ans[i] = SmoothingHas0Len5(aa, centroidRadio, repeat);
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
		ans[i] = atan2(vec.x, vec.y) * M_1_PI * 180;
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
		double angle = atan2(vec.x, vec.y) * M_1_PI * 180;
		ans.push_back(Endpoint(nowLine.front(), i, 0, angle));
		int last = (int)nowLine.size() - 1;
		vec = nowLine[last] - nowLine[last - 2];
		angle = atan2(vec.x, vec.y) * M_1_PI * 180;
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

bool CheckEndpointSimilarity(const Endpoint& lhs, const Endpoint& rhs,
							 double angle)
{
	Vector2 vec = lhs.pos - rhs.pos;
	double linkAngle = atan2(vec.x, vec.y) * M_1_PI * 180;
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
			if (nearEps[j] != eps[i] && CheckEndpointSimilarity(nearEps[j], eps[i], angle))
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
			//nowWidths = FixLineWidths(nowWidths, 200);
		}
	}
}

LineEnds GetLineEnds(const Lines& cvp)
{
	LineEnds ans;
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Line& nowLine = cvp.at(i);
		int last = (int)nowLine.size() - 1;
		Vector2 p1 = nowLine[0] - nowLine[2];
		Vector2 p2 = nowLine[last] - nowLine[last - 2];
		double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
		double angle2 = fmod(360 + atan2(p2.x, p2.y) * M_1_PI * 180, 360);
		ans.push_back(LineEnd(nowLine.front(), nowLine.back(), i, last, angle1,
							  angle2));
	}
	return ans;
}

void LinkLineEnds180(LineEnds& les, double distance, double angle)
{
	double sdistance = distance * distance;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		LineEnd& le1 = les[i];
		for (int j = 0; j < (int)les.size(); ++j)
		{
			if (i != j)
			{
				bool haslink = false;
				LineEnd& le2 = les[j];
				// find begin link
				if (le1.beg.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity180(le1, le2, LineEnd::BEGIN_TO_BEGIN, angle))
					{
						le1.beglinks.push_back(j);
						le2.beglinks.push_back(i);
						le1.beglink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.beg.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity180(le1, le2, LineEnd::BEGIN_TO_END, angle))
					{
						le1.beglinks.push_back(j);
						le2.endlinks.push_back(i);
						le1.beglink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				// find end link
				if (le1.end.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity180(le1, le2, LineEnd::END_TO_BEGIN, angle))
					{
						le1.endlinks.push_back(j);
						le2.beglinks.push_back(i);
						le1.endlink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.end.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity180(le1, le2, LineEnd::END_TO_END, angle))
					{
						le1.endlinks.push_back(j);
						le2.endlinks.push_back(i);
						le1.endlink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				if (haslink)
				{
					if (le1.linkIdx1)
					{
						le1.linkIdx2 = &le2;
					}
					else
					{
						le1.linkIdx1 = &le2;
					}
					if (le2.linkIdx1)
					{
						le2.linkIdx2 = &le1;
					}
					else
					{
						le2.linkIdx1 = &le1;
					}
				}
			}
		}
	}
}


void LinkLineEnds(LineEnds& les, double distance, double angle)
{
	double sdistance = distance * distance;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		LineEnd& le1 = les[i];
		for (int j = 0; j < (int)les.size(); ++j)
		{
			if (i != j)
			{
				bool haslink = false;
				LineEnd& le2 = les[j];
				// find begin link
				if (le1.beg.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::BEGIN_TO_BEGIN, angle))
					{
						le1.beglinks.push_back(j);
						le2.beglinks.push_back(i);
						le1.beglink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.beg.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::BEGIN_TO_END, angle))
					{
						le1.beglinks.push_back(j);
						le2.endlinks.push_back(i);
						le1.beglink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				// find end link
				if (le1.end.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::END_TO_BEGIN, angle))
					{
						le1.endlinks.push_back(j);
						le2.beglinks.push_back(i);
						le1.endlink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.end.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::END_TO_END, angle))
					{
						le1.endlinks.push_back(j);
						le2.endlinks.push_back(i);
						le1.endlink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				if (haslink)
				{
					if (le1.linkIdx1)
					{
						le1.linkIdx2 = &le2;
					}
					else
					{
						le1.linkIdx1 = &le2;
					}
					if (le2.linkIdx1)
					{
						le2.linkIdx2 = &le1;
					}
					else
					{
						le2.linkIdx1 = &le1;
					}
				}
			}
		}
	}
}

bool CheckLinkEnd_Similarity(const LineEnd& lhs, const LineEnd& rhs,
							 LineEnd::LinkMethod c, double angle)
{
	double rangleBeg = rhs.angleBeg + 360;
	double rangleEnd = rhs.angleEnd + 360;
	double rangleBeg2 = fmod(rangleBeg, 360);
	double rangleEnd2 = fmod(rangleEnd, 360);
	switch (c)
	{
	case LineEnd::BEGIN_TO_BEGIN:
		if (abs(lhs.angleBeg - rangleBeg) < angle || abs(lhs.angleBeg - rangleBeg2) < angle)
		{
			Vector2 p1 = rhs.beg - lhs.beg;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (abs(lhs.angleBeg - angle1) < angle)
			{
				return true;
			}
		}
		break;
	case LineEnd::BEGIN_TO_END:
		if (abs(lhs.angleBeg - rangleEnd) < angle || abs(lhs.angleBeg - rangleEnd2) < angle)
		{
			Vector2 p1 = rhs.end - lhs.beg;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (abs(lhs.angleBeg - angle1) < angle)
			{
				return true;
			}
		}
		break;
	case LineEnd::END_TO_BEGIN:
		if (abs(lhs.angleEnd - rangleBeg) < angle || abs(lhs.angleEnd - rangleBeg2) < angle)
		{
			Vector2 p1 = rhs.beg - lhs.end;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (abs(lhs.angleEnd - angle1) < angle)
			{
				return true;
			}
		}
		break;
	case LineEnd::END_TO_END:
		if (abs(lhs.angleEnd - rangleEnd) < angle || abs(lhs.angleEnd - rangleEnd2) < angle)
		{
			Vector2 p1 = rhs.end - lhs.end;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (abs(lhs.angleEnd - angle1) < angle)
			{
				return true;
			}
		}
		break;
	}
	return false;
}

bool CheckAngle(double a1, double a2, double limitA)
{
	if (a2 < a1)
	{
		double t = a1;
		a1 = a2;
		a2 = t;
	}
	if (abs(a1 - a2) < limitA || abs((a1 + 360) - a2) < limitA)
	{
		return true;
	}
	return false;
}


bool CheckLinkEnd_Similarity180(const LineEnd& lhs, const LineEnd& rhs,
								LineEnd::LinkMethod c, double angle)
{
	double rangleBeg = fmod(rhs.angleBeg + 180, 360);
	double rangleEnd = fmod(rhs.angleEnd + 180, 360);
	switch (c)
	{
	case LineEnd::BEGIN_TO_BEGIN:
		if (CheckAngle(lhs.angleBeg, rangleBeg, angle))
		{
			Vector2 p1 = rhs.beg - lhs.beg;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (CheckAngle(lhs.angleBeg, angle1, angle) &&
					CheckAngle(rangleBeg, angle1, angle))
			{
				return true;
			}
		}
		break;
	case LineEnd::BEGIN_TO_END:
		if (CheckAngle(lhs.angleBeg, rangleEnd, angle))
		{
			Vector2 p1 = rhs.end - lhs.beg;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (CheckAngle(lhs.angleBeg, angle1, angle) &&
					CheckAngle(rangleEnd, angle1, angle))
			{
				return true;
			}
		}
		break;
	case LineEnd::END_TO_BEGIN:
		if (CheckAngle(lhs.angleEnd, rangleBeg, angle))
		{
			Vector2 p1 = rhs.beg - lhs.end;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (CheckAngle(lhs.angleEnd, angle1, angle) &&
					CheckAngle(rangleBeg, angle1, angle))
			{
				return true;
			}
		}
		break;
	case LineEnd::END_TO_END:
		if (CheckAngle(lhs.angleEnd, rangleEnd, angle))
		{
			Vector2 p1 = rhs.end - lhs.end;
			double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
			if (CheckAngle(lhs.angleEnd, angle1, angle) &&
					CheckAngle(rangleEnd, angle1, angle))
			{
				return true;
			}
		}
		break;
	}
	return false;
}

void ConnectLineEnds(LineEnds& les, Lines& pos, Lines& width)
{
	std::vector<bool> marked(pos.size(), false);
	Lines newpos;
	Lines newwidth;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		LineEnd& le1 = les[i];
		if (le1.linkIdx1 != 0 && le1.linkIdx2 == 0)
		{
			if (!marked[le1.idx])
			{
				Line addline;
				Line addwidth;
				LineEnd* nowle = &le1;
				LineEnd* last = 0;
				LineEnd_ptrs testle;
				while (nowle)
				{
					testle.push_back(nowle);
					marked[nowle->idx] = true;
					Line& loopline = pos[nowle->idx];
					Line& loopwidth = width[nowle->idx];
					LineEnd* next = 0;
					if (last == 0)
					{
						next = nowle->linkIdx1;
					}
					else if (last == nowle->linkIdx1)
					{
						next = nowle->linkIdx2;
					}
					else
					{
						next = nowle->linkIdx1;
					}
					if (testle.end() != std::find(testle.begin(), testle.end(), next))
					{
						break;
					}
					if (last == nowle->endlink)
					{
						addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
						addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					}
					else if (last == nowle->beglink)
					{
						addline.insert(addline.end(), loopline.begin(), loopline.end());
						addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					}
					else if (next == nowle->beglink)
					{
						addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
						addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					}
					else if (next == nowle->endlink)
					{
						addline.insert(addline.end(), loopline.begin(), loopline.end());
						addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					}
					last = nowle;
					nowle = next;
				}
				newpos.push_back(addline);
				newwidth.push_back(addwidth);
			}
		}
	}
	for (int i = 0; i < (int)les.size(); ++i)
	{
		if (!marked[i])
		{
			newpos.push_back(pos[i]);
			newwidth.push_back(width[i]);
		}
	}
	pos = newpos;
	width = newwidth;
}

void ConnectLineEnds2(const LineEnds& les, Lines& pos, Lines& width)
{
	std::vector<bool> marked(pos.size(), false);
	Lines newpos;
	Lines newwidth;
	for (LineEnds::const_iterator it1 = les.begin(); it1 != les.end(); ++it1)
	{
		const LineEnd& le1 = *it1;
		if (!le1.beglinks.empty() && le1.endlinks.empty())
		{
			int nowid = le1.idx;
			if (marked[nowid])
			{
				continue;
			}
			Line addline;
			Line addwidth;
			addline.insert(addline.end(), pos[nowid].rbegin(), pos[nowid].rend());
			addwidth.insert(addwidth.end(), width[nowid].rbegin(), width[nowid].rend());
			marked[nowid] = true;
			nowid = le1.beglinks.front();
			while (nowid != -1 && !marked[nowid])
			{
				marked[nowid] = true;
				int next = nowid;
				const Line& loopline = pos[nowid];
				const Line& loopwidth = width[nowid];
				double dis1 = loopline.front().distance(addline.back());
				double dis2 = loopline.back().distance(addline.back());
				if (dis1 < dis2)
				{
					addline.insert(addline.end(), loopline.begin(), loopline.end());
					addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					if (!les[nowid].endlinks.empty())
					{
						next = les[nowid].endlinks.back();
					}
				}
				else
				{
					addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
					addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					if (!les[nowid].beglinks.empty())
					{
						next = les[nowid].beglinks.front();
					}
				}
				if (next == nowid)
				{
					nowid = -1;
				}
			}
			newpos.push_back(addline);
			newwidth.push_back(addwidth);
		}
		else if (le1.beglinks.empty() && !le1.endlinks.empty())
		{
			int nowid = le1.idx;
			if (marked[nowid])
			{
				continue;
			}
			Line addline;
			Line addwidth;
			addline.insert(addline.end(), pos[nowid].begin(), pos[nowid].end());
			addwidth.insert(addwidth.end(), width[nowid].begin(), width[nowid].end());
			marked[nowid] = true;
			nowid = le1.endlinks.front();
			while (nowid != -1 && !marked[nowid])
			{
				marked[nowid] = true;
				int next = nowid;
				const Line& loopline = pos[nowid];
				const Line& loopwidth = width[nowid];
				double dis1 = loopline.front().distance(addline.back());
				double dis2 = loopline.back().distance(addline.back());
				if (dis1 < dis2)
				{
					addline.insert(addline.end(), loopline.begin(), loopline.end());
					addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					if (!les[nowid].endlinks.empty())
					{
						next = les[nowid].endlinks.back();
					}
				}
				else
				{
					addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
					addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					if (!les[nowid].beglinks.empty())
					{
						next = les[nowid].beglinks.front();
					}
				}
				if (next == nowid)
				{
					nowid = -1;
				}
			}
			newpos.push_back(addline);
			newwidth.push_back(addwidth);
		}
	}
	for (int i = 0; i < (int)les.size(); ++i)
	{
		if (!marked[i])
		{
			newpos.push_back(pos[i]);
			newwidth.push_back(width[i]);
		}
	}
	pos = newpos;
	width = newwidth;
}

void ConnectLineEnds3(const LineEnds& les, Lines& pos, Lines& width)
{
	std::vector<bool> marked(pos.size(), false);
	Lines newpos;
	Lines newwidth;
	for (LineEnds::const_iterator it1 = les.begin(); it1 != les.end(); ++it1)
	{
		const LineEnd& le1 = *it1;
		if (!le1.beglinks.empty())
		{
			int nowid = le1.idx;
			if (marked[nowid])
			{
				continue;
			}
			Line addline;
			Line addwidth;
			addline.insert(addline.end(), pos[nowid].rbegin(), pos[nowid].rend());
			addwidth.insert(addwidth.end(), width[nowid].rbegin(), width[nowid].rend());
			marked[nowid] = true;
			nowid = le1.beglinks.front();
			while (nowid != -1 && !marked[nowid])
			{
				marked[nowid] = true;
				int next = nowid;
				const Line& loopline = pos[nowid];
				const Line& loopwidth = width[nowid];
				double dis1 = loopline.front().distance(addline.back());
				double dis2 = loopline.back().distance(addline.back());
				if (dis1 < dis2)
				{
					addline.insert(addline.end(), loopline.begin(), loopline.end());
					addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					if (!les[nowid].endlinks.empty())
					{
						next = les[nowid].endlinks.back();
					}
				}
				else
				{
					addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
					addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					if (!les[nowid].beglinks.empty())
					{
						next = les[nowid].beglinks.front();
					}
				}
				if (next == nowid)
				{
					nowid = -1;
				}
			}
			newpos.push_back(addline);
			newwidth.push_back(addwidth);
		}
		else if (!le1.endlinks.empty())
		{
			int nowid = le1.idx;
			if (marked[nowid])
			{
				continue;
			}
			Line addline;
			Line addwidth;
			addline.insert(addline.end(), pos[nowid].begin(), pos[nowid].end());
			addwidth.insert(addwidth.end(), width[nowid].begin(), width[nowid].end());
			marked[nowid] = true;
			nowid = le1.endlinks.front();
			while (nowid != -1 && !marked[nowid])
			{
				marked[nowid] = true;
				int next = nowid;
				const Line& loopline = pos[nowid];
				const Line& loopwidth = width[nowid];
				double dis1 = loopline.front().distance(addline.back());
				double dis2 = loopline.back().distance(addline.back());
				if (dis1 < dis2)
				{
					addline.insert(addline.end(), loopline.begin(), loopline.end());
					addwidth.insert(addwidth.end(), loopwidth.begin(), loopwidth.end());
					if (!les[nowid].endlinks.empty())
					{
						next = les[nowid].endlinks.back();
					}
				}
				else
				{
					addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
					addwidth.insert(addwidth.end(), loopwidth.rbegin(), loopwidth.rend());
					if (!les[nowid].beglinks.empty())
					{
						next = les[nowid].beglinks.front();
					}
				}
				if (next == nowid)
				{
					nowid = -1;
				}
			}
			newpos.push_back(addline);
			newwidth.push_back(addwidth);
		}
	}
	for (int i = 0; i < (int)les.size(); ++i)
	{
		if (!marked[i])
		{
			newpos.push_back(pos[i]);
			newwidth.push_back(width[i]);
		}
	}
	pos = newpos;
	width = newwidth;
}

void ConnectNearestLines(const LineEnds& les, Lines& pos, Lines& width, double d2,
						 double angle)
{
	d2 *= d2;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		const LineEnd& le1 = les[i];
		int minbegp = -1, minbeg_angle = angle, minbeg_dis = d2, minbeg_id = -1;
		int minendp = -1, minend_angle = angle, minend_dis = d2, minend_id = -1;
		double hitbeg_width = 0, hitend_width = 0;
		Vector2 begdst, enddst;
		for (int j = 0; j < pos.size(); ++j)
		{
			Line& nowLine = pos[j];
			Line& nowWidth = width[j];
			for (int k = 0; k < nowLine.size(); k ++)
			{
				if (le1.idx == j)
				{
					continue;
				}
				int dis1 = le1.beg.squaredDistance(nowLine[k]);
				int dis2 = le1.end.squaredDistance(nowLine[k]);
				if (dis1 < minbeg_dis && !le1.haslink1)
				{
					Vector2 p1 = nowLine[k] - le1.beg;
					double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
					angle1 = abs(le1.angleBeg - angle1);
					if (dis1 < minbeg_dis && angle1 < angle)
					{
						minbeg_angle = angle1;
						minbeg_dis = dis1;
						minbegp = k;
						begdst = nowLine[minbegp];
						hitbeg_width = nowWidth[minbegp].x + nowWidth[minbegp].y;
						minbeg_id = j;
					}
				}
				if (dis2 < minend_dis && !le1.haslink2)
				{
					Vector2 p1 = nowLine[k] - le1.end;
					double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
					angle1 = abs(le1.angleEnd - angle1);
					if (dis2 < minend_dis && angle1 < angle)
					{
						minend_angle = angle1;
						minend_dis = dis2;
						minendp = k;
						enddst = nowLine[minendp];
						hitend_width = nowWidth[minendp].x + nowWidth[minendp].y;
						minend_id = j;
					}
				}
			}
		}
		const int ELONGATION_LEN = 40;
		if (minbegp != -1)
		{
			const Line& orgin = pos[minbeg_id];
			const Line& orginw = width[minbeg_id];
			Line& elongation = pos[le1.idx];
			Line& elongationw = width[le1.idx];
			const Line normals = GetNormalsLen2(orgin);
			elongation.insert(elongation.begin(), begdst);
			elongationw.insert(elongationw.begin(), elongationw.front());
			//if (hitbeg_width > 0.5)
			{
				FixBeginWidth(elongationw, 20);
			}
			int orgin_id1 = minbegp - 2;
			int orgin_id2 = minbegp + 2;
			if (orgin_id1 >= 0 && orgin_id2 < orgin.size())
			{
				Vector2 n1 = orgin[minbegp] + normals[minbegp] * 2;
				Vector2 n2 = orgin[minbegp] - normals[minbegp] * 2;
				Vector2 vec1 = orgin[orgin_id1] - orgin[orgin_id2];
				Vector2 vec2 = elongation[0] - elongation[1];
				vec1.normalise();
				vec2.normalise();
				double side_angle1 = acos(vec1.dotProduct(vec2)) * 180 * M_1_PI;
				double side_angle2 = acos(vec1.dotProduct(-vec2)) * 180 * M_1_PI;
				if (side_angle1 < 45 || side_angle2 < 45)
				{
					double dis1 = orgin[orgin_id1].squaredDistance(elongation[0] + vec2 * 2);
					double dis2 = orgin[orgin_id2].squaredDistance(elongation[0] + vec2 * 2);
					double ndis1 = n1.squaredDistance(elongation[1]);
					double ndis2 = n2.squaredDistance(elongation[1]);
					double nlen = orgin[minbegp].distance(elongation[1]);
					for (int h = minendp - 20; h < minendp + 20 && h < orgin.size(); ++h)
					{
						if (h < 0)
						{
							h = 0;
						}
						double tlen = orgin[h].distance(elongation[1]);
						if (tlen < nlen)
						{
							nlen = tlen;
						}
					}
					nlen *= 0.3;
					if (dis1 < dis2 && orgin_id1 < ELONGATION_LEN)
					{
						elongation.erase(elongation.begin());
						elongationw.erase(elongationw.begin());
						if (ndis1 < ndis2)
						{
							for (int i = minbegp; i >= 0; --i)
							{
								elongationw.insert(elongationw.begin(), elongationw.front());
								elongation.insert(elongation.begin(), orgin[i] + normals[i] *nlen * (i / (double)minbegp));
							}
						}
						else
						{
							for (int i = minbegp; i >= 0; --i)
							{
								elongationw.insert(elongationw.begin(), elongationw.front());
								elongation.insert(elongation.begin(), orgin[i] - normals[i] *nlen * (i / (double)minbegp));
							}
						}
						//elongation.insert(elongation.begin(), orgin.front());
					}
					else if (dis1 < dis2 && (orgin.size() - orgin_id1) < ELONGATION_LEN)
					{
						elongation.erase(elongation.begin());
						elongationw.erase(elongationw.begin());
						int div = orgin.size() - minbegp;
						if (ndis1 < ndis2)
						{
							for (int i = minbegp; i < orgin.size(); ++i)
							{
								elongationw.insert(elongationw.begin(), elongationw.front());
								elongation.insert(elongation.begin(),
												  orgin[i] + normals[i] *nlen * ((orgin.size() - i) / (double)div));
							}
						}
						else
						{
							for (int i = minbegp; i < orgin.size(); ++i)
							{
								elongationw.insert(elongationw.begin(), elongationw.front());
								elongation.insert(elongation.begin(),
												  orgin[i] - normals[i] *nlen * ((orgin.size() - i) / (double)div));
							}
						}
						//elongation.insert(elongation.begin(), orgin.back());
					}
				}
			}
		}
		if (minendp != -1)
		{
			Line& orgin = pos[minend_id];
			Line& orginw = width[minend_id];
			Line& elongation = pos[le1.idx];
			Line& elongationw = width[le1.idx];
			Line normals = GetNormalsLen2(orgin);
			elongation.insert(elongation.end(), enddst);
			elongationw.insert(elongationw.end(), elongationw.back());
			//if (hitend_width > 0.5)
			{
				FixEndWidth(elongationw, 20);
			}
			int orgin_id1 = minendp - 2;
			int orgin_id2 = minendp + 2;
			if (orgin_id1 >= 0 && orgin_id2 < orgin.size())
			{
				Vector2 n1 = orgin[minendp] + normals[minendp] * 2;
				Vector2 n2 = orgin[minendp] - normals[minendp] * 2;
				int last = elongation.size() - 1;
				Vector2 vec1 = orgin[orgin_id1] - orgin[orgin_id2];
				Vector2 vec2 = elongation[last] - elongation[last - 1];
				vec1.normalise();
				vec2.normalise();
				double side_angle1 = acos(vec1.dotProduct(vec2)) * 180 * M_1_PI;
				double side_angle2 = acos(vec1.dotProduct(-vec2)) * 180 * M_1_PI;
				if (side_angle1 < 45 || side_angle2 < 45)
				{
					double dis1 = orgin[orgin_id1].squaredDistance(elongation[last] + vec2 * 2);
					double dis2 = orgin[orgin_id2].squaredDistance(elongation[last] + vec2 * 2);
					double ndis1 = n1.squaredDistance(elongation[last - 1]);
					double ndis2 = n2.squaredDistance(elongation[last - 1]);
					double nlen = orgin[minendp].distance(elongation[last - 1]);
					for (int h = minendp - 20; h < minendp + 20 && h < orgin.size(); ++h)
					{
						if (h < 0)
						{
							h = 0;
						}
						double tlen = orgin[h].distance(elongation[last - 1]);
						if (tlen < nlen)
						{
							nlen = tlen;
						}
					}
					nlen *= 0.4;
					if (dis1 < dis2 && orgin_id1 < ELONGATION_LEN)
					{
						elongation.erase(elongation.end() - 1);
						elongationw.erase(elongationw.end() -1);
						if (ndis1 < ndis2)
						{
							for (int i = minendp; i >= 0; --i)
							{
								elongationw.insert(elongationw.end(), elongationw.back());
								elongation.insert(elongation.end(), orgin[i] + normals[i] *nlen * (i / (double)minendp));
							}
						}
						else
						{
							for (int i = minendp; i >= 0; --i)
							{
								elongationw.insert(elongationw.end(), elongationw.back());
								elongation.insert(elongation.end(), orgin[i] - normals[i] *nlen * (i / (double)minendp));
							}
						}
						//elongation.insert(elongation.end(), orgin.front());
					}
					else if (dis1 > dis2 && (orgin.size() - orgin_id1) < ELONGATION_LEN)
					{
						elongation.erase(elongation.end() - 1);
						elongationw.erase(elongationw.end() -1);
						int div = orgin.size() - minendp;
						if (ndis1 < ndis2)
						{
							for (int i = minendp; i < orgin.size(); ++i)
							{
								elongationw.insert(elongationw.end(), elongationw.back());
								elongation.insert(elongation.end(),
												  orgin[i] + normals[i]*nlen * ((orgin.size() - i) / (double)div));
							}
						}
						else
						{
							for (int i = minendp; i < orgin.size(); ++i)
							{
								elongationw.insert(elongationw.end(), elongationw.back());
								elongation.insert(elongation.end(),
												  orgin[i] - normals[i]*nlen * ((orgin.size() - i) / (double)div));
							}
						}
						//elongation.insert(elongation.end(), orgin.back());
					}
				}
			}
		}
	}
}
void ConnectNearestLines(const LineEnds& les, Lines& pos, Color2Side& width, double d2,
						 double angle)
{
	d2 *= d2;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		const LineEnd& le1 = les[i];
		int minbegp = -1, minbeg_angle = angle, minbeg_dis = d2;
		int minendp = -1, minend_angle = angle, minend_dis = d2;
		Vector2 begdst, enddst;
		for (int j = 0; j < pos.size(); ++j)
		{
			Line& nowLine = pos[j];
			for (int k = 0; k < nowLine.size(); k ++)
			{
				int dis1 = le1.beg.squaredDistance(nowLine[k]);
				int dis2 = le1.end.squaredDistance(nowLine[k]);
				if (dis1 < minbeg_dis && !le1.haslink1)
				{
					Vector2 p1 = nowLine[k] - le1.beg;
					double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
					angle1 = abs(le1.angleBeg - angle1);
					if (dis1 < minbeg_dis &&  angle1 < minbeg_angle)
					{
						minbeg_angle = angle1;
						minbeg_dis = dis1;
						minbegp = k;
						begdst = nowLine[minbegp];
					}
				}
				if (dis2 < minend_dis && !le1.haslink2)
				{
					Vector2 p1 = nowLine[k] - le1.end;
					double angle1 = fmod(360 + atan2(p1.x, p1.y) * M_1_PI * 180, 360);
					angle1 = abs(le1.angleEnd - angle1);
					if (dis2 < minend_dis && angle1 < minend_angle)
					{
						minend_angle = angle1;
						minend_dis = dis2;
						minendp = k;
						enddst = nowLine[minendp];
					}
				}
			}
		}
		if (minbegp != -1)
		{
			Line& elongation = pos[le1.idx];
			Vector3s& elongationw1 = width.left[le1.idx];
			Vector3s& elongationw2 = width.right[le1.idx];
			elongation.insert(elongation.begin(), begdst);
			elongationw1.insert(elongationw1.begin(), elongationw1.front());
			elongationw2.insert(elongationw2.begin(), elongationw2.front());
			{
				FixBeginWidth(elongationw1, 20);
				FixBeginWidth(elongationw2, 20);
			}
		}
		if (minendp != -1)
		{
			Line& elongation = pos[le1.idx];
			Vector3s& elongationw1 = width.left[le1.idx];
			Vector3s& elongationw2 = width.right[le1.idx];
			elongation.insert(elongation.end(), enddst);
			elongationw1.insert(elongationw1.end(), elongationw1.back());
			elongationw2.insert(elongationw2.end(), elongationw2.back());
			{
				FixEndWidth(elongationw1, 20);
				FixEndWidth(elongationw2, 20);
			}
		}
	}
}
void IncreaseDensity(Line& pos, Line& pos2)
{
	{
		Line ans(pos.size() * 2 - 1);
		for (int i = 0; i < (int)pos.size() - 1; ++i)
		{
			Vector2& v1 = pos[i];
			Vector2& v2 = pos[i + 1];
			ans[i * 2] = v1;
			ans[i * 2 + 1] = (v2 + v1) * 0.5;
		}
		ans.back() = pos.back();
		pos = ans;
	}
	{
		Line ans(pos2.size() * 2 - 1);
		for (int i = 0; i < (int)pos2.size() - 1; ++i)
		{
			Vector2& v1 = pos2[i];
			Vector2& v2 = pos2[i + 1];
			ans[i * 2] = v1;
			ans[i * 2 + 1] = (v2 + v1) * 0.5;
		}
		ans.back() = pos2.back();
		pos2 = ans;
	}
}
void IncreaseDensity(Lines& pos, Lines& pos2)
{
	for (int i = 0; i < pos.size(); ++i)
	{
		Line& nowLine = pos[i];
		Line& nowLine2 = pos2[i];
		IncreaseDensity(nowLine, nowLine2);
	}
}
void FixBeginWidth(Line& width, int len)
{
	int sidx = 0;
	int eidx = len;
	if (eidx >= width.size())
	{
		eidx = (int)width.size() - 1;
	}
	double max_width = 0.5;
	int widx = -1;
	Vector2 w;
	for (int i = sidx; i < eidx; ++i)
	{
		Vector2& v1 = width[i];
		if (v1.x + v1.y > max_width)
		{
			max_width = v1.x + v1.y;
			widx = i;
			w = v1;
		}
	}
	if (widx != -1)
	{
		for (int j = sidx; j < widx; ++j)
		{
			width[j] = w;
		}
	}
}
void FixBeginWidth(Vector3s& width, int len)
{
	int sidx = 0;
	int eidx = len;
	if (eidx >= width.size())
	{
		eidx = (int)width.size() - 1;
	}
	double max_width = 0.5;
	int widx = -1;
	Vector3 w;
	for (int i = sidx; i < eidx; ++i)
	{
		Vector3& v1 = width[i];
		if (v1.x + v1.y > max_width)
		{
			max_width = v1.x + v1.y;
			widx = i;
			w = v1;
		}
	}
	if (widx != -1)
	{
		for (int j = sidx; j < widx; ++j)
		{
			width[j] = w;
		}
	}
}
void FixEndWidth(Line& width, int len)
{
	int sidx = (int)width.size() - 1;
	int eidx = (int)width.size() - 1 - len;
	if (eidx < 0)
	{
		eidx = 0;
	}
	double max_width = 0.5;
	int widx = -1;
	Vector2 w;
	for (int i = sidx; i > eidx ; --i)
	{
		Vector2& v1 = width[i];
		if (v1.x + v1.y > max_width)
		{
			max_width = v1.x + v1.y;
			widx = i;
			w = v1;
		}
	}
	if (widx != -1)
	{
		for (int j = (int)sidx; j > widx; --j)
		{
			width[j] = w;
		}
	}
}
void FixEndWidth(Vector3s& width, int len)
{
	int sidx = (int)width.size() - 1;
	int eidx = (int)width.size() - 1 - len;
	if (eidx < 0)
	{
		eidx = 0;
	}
	double max_width = 0.5;
	int widx = -1;
	Vector3 w;
	for (int i = sidx; i > eidx ; --i)
	{
		Vector3& v1 = width[i];
		if (v1.x + v1.y > max_width)
		{
			max_width = v1.x + v1.y;
			widx = i;
			w = v1;
		}
	}
	if (widx != -1)
	{
		for (int j = (int)sidx; j > widx; --j)
		{
			width[j] = w;
		}
	}
}
Vector3s FixLineColors(const Vector3s& cvp, int range, int findlimit)
{
	Vector3s cps = cvp;
	for (int i = 0; i < cvp.size(); ++i)
	{
		if (cvp[i].length() < 1)
		{
			Vector3 setValueX;
			int finds = 0, j = 1;
			for (; j < range && finds < findlimit; ++j)
			{
				int left = i - j;
				int right = i + j;
				if (left > 0 && cvp[left].length() > 1)
				{
					finds++;
					setValueX += cvp[left];
				}
				if (right < cvp.size() && cvp[right].length() > 1)
				{
					finds++;
					setValueX += cvp[right];
				}
			}
			if (finds > 0)
			{
				setValueX /= finds;
				cps[i] = setValueX;
			}
		}
	}
	return cps;
}
Vector3s2d FixLineColors(const Vector3s2d& aLine, int range, int findlimit)
{
	Vector3s2d ans(aLine.size());
	for (int i = 0; i < aLine.size(); ++i)
	{
		const Vector3s& aa = aLine.at(i);
		ans[i] = FixLineColors(aa, range, findlimit);
	}
	return ans;
}
double GetLightV3P(const Vector3* v)
{
	return v[0][0] * 0.299 + v[0][1] * 0.587 + v[0][2] * 0.114;
}
bool LightCompareV3P(const Vector3* v1, const Vector3* v2)
{
	return GetLightV3P(v1) < GetLightV3P(v2);
}
typedef std::vector<Vector3*> Vector3Ps;
Vector3s HistormMappingLight(const Vector3s& color, double radio)
{
	Vector3Ps tmp(color.size());
	for (int i = 0; i < color.size(); ++i)
	{
		tmp[i] = const_cast<Vector3*>(&color[i]);
	}
	std::sort(tmp.begin(), tmp.end(), LightCompareV3P);
	int left_idx = (1 - radio) * 0.5 * color.size();
	int right_idx = color.size() - left_idx - 1;
	Vector3s ans(color.size());
	double step = (right_idx - left_idx + 1) / (double)color.size();
	for (int i = 0; i < color.size(); ++i)
	{
		int oidx = tmp[i] - &color[0];
		int idx = left_idx + i * step;
		ans[oidx] = *tmp[idx];
	}
	return ans;
}
Vector3s2d HistormMappingLight(const Vector3s2d& color, double radio)
{
	Vector3s2d ans(color.size());
	for (int i = 0; i < color.size(); ++i)
	{
		const Vector3s& cc = color.at(i);
		ans[i] = HistormMappingLight(cc, radio);
	}
	return ans;
}
Vector3s HistormMappingHSV(const Vector3s& color, double radio)
{
	Vector3s ans(color.size());
	return ans;
}
ints MedianLen5(const ints& cvp, int repeat /*= 1*/)
{
	ints cps = cvp;
	ints newcps;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		for (int i = 0; i < cps.size(); ++i)
		{
			int s_idx = i - 2;
			int e_idx = i + 3;
			if (s_idx < 0)
			{
				s_idx = 0;
			}
			if (e_idx - s_idx < 5)
			{
				e_idx = s_idx + 5;
			}
			if (e_idx >= cvp.size())
			{
				e_idx = cvp.size() - 1;
				s_idx = e_idx - 5;
			}
			if (s_idx < 0)
			{
				s_idx = 0;
			}
			ints tmp;
			tmp.insert(tmp.end(), cps.begin() + s_idx, cps.begin() + e_idx);
			std::sort(tmp.begin(), tmp.end());
			newcps.push_back(tmp[tmp.size() / 2]);
		}
		cps = newcps;
	}
	return cps;
}
ints2d MedianLen5(const ints2d& cvp, int repeat /*= 1*/)
{
	ints2d cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const ints& nowLine = cvp[i];
		cps[i] = MedianLen5(nowLine, repeat);
	}
	return cps;
}
Vector3s MedianLen5(const Vector3s& cvp, int repeat /*= 1*/)
{
	Vector3s cps = cvp;
	Vector3s newcps;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		for (int i = 0; i < cps.size(); ++i)
		{
			int s_idx = i - 2;
			int e_idx = i + 3;
			if (s_idx < 0)
			{
				s_idx = 0;
			}
			if (e_idx - s_idx < 5)
			{
				e_idx = s_idx + 5;
			}
			if (e_idx >= cvp.size())
			{
				e_idx = cvp.size() - 1;
				s_idx = e_idx - 5;
			}
			Vector3s tmp;
			tmp.insert(tmp.end(), cps.begin() + s_idx, cps.begin() + e_idx);
			std::sort(tmp.begin(), tmp.end());
			newcps.push_back(tmp[tmp.size() / 2]);
		}
		cps = newcps;
	}
	return cps;
}
Vector3s2d MedianLen5(const Vector3s2d& cvp, int repeat /*= 1*/)
{
	Vector3s2d cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Vector3s& nowLine = cvp[i];
		cps[i] = MedianLen5(nowLine, repeat);
	}
	return cps;
}
Vector3s MedianLen(const Vector3s& cvp, int len, int repeat /*= 1*/)
{
	if (cvp.size() < 3)
	{
		return cvp;
	}
	Vector3s cps = cvp;
	Vector3s newcps;
	const int mius = (len - 1) / 2;
	const int add = (len + 1) / 2;
	for (int repeatCount = 0; repeatCount < repeat; repeatCount++)
	{
		newcps.clear();
		for (int i = 0; i < cps.size(); ++i)
		{
			int s_idx = i - mius;
			int e_idx = i + add;
			if (s_idx < 0)
			{
				s_idx = 0;
			}
			if (e_idx - s_idx < len)
			{
				e_idx = s_idx + len;
			}
			if (e_idx >= cvp.size())
			{
				e_idx = cvp.size() - 1;
				s_idx = e_idx - len;
			}
			if (s_idx < 0)
			{
				s_idx = 0;
			}
			Vector3s tmp;
			tmp.insert(tmp.end(), cps.begin() + s_idx, cps.begin() + e_idx);
			std::sort(tmp.begin(), tmp.end());
			newcps.push_back(tmp[tmp.size() / 2]);
		}
		cps = newcps;
	}
	return cps;
}
Vector3s2d MedianLen(const Vector3s2d& cvp, int len, int repeat /*= 1*/)
{
	Vector3s2d cps(cvp.size());
	for (int i = 0; i < cvp.size(); ++i)
	{
		const Vector3s& nowLine = cvp[i];
		cps[i] = MedianLen(nowLine, len, repeat);
	}
	return cps;
}
void ClearJointArea(const LineEnds& les, Lines& pos, Color2Side& color2s, double len)
{
	double sdistance = len * len;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		const LineEnd& le1 = les[i];
		Vector3s2d& colorls2d = color2s.left;
		Vector3s2d& colorrs2d = color2s.right;
		for (int j = 0; j < (int)pos.size(); ++j)
		{
			Line& line = pos[j];
			Vector3s& colorls = colorls2d[j];
			Vector3s& colorrs = colorrs2d[j];
			if (line.size() > 3)
			{
				for (int k = 0; k < (int)line.size(); ++k)
				{
					if (line[k].squaredDistance(le1.beg) < sdistance)
					{
						if (k > 0 && k < line.size() - 1)
						{
							if (colorls[k + 1] == Vector3::ZERO &&
									colorls[k - 1] == Vector3::ZERO)
							{
								continue;
							}
						}
						colorls[k] = Vector3::ZERO;
						colorrs[k] = Vector3::ZERO;
					}
					else if (line[k].squaredDistance(le1.end) < sdistance)
					{
						if (k > 0 && k < line.size() - 1)
						{
							if (colorls[k + 1] == Vector3::ZERO &&
									colorls[k - 1] == Vector3::ZERO)
							{
								continue;
							}
						}
						colorls[k] = Vector3::ZERO;
						colorrs[k] = Vector3::ZERO;
					}
				}
			}
		}
	}
}
void ConnectSimilarColor2Side(const LineEnds& les, Lines& pos, Color2Side& width)
{
	std::vector<bool> marked(pos.size(), false);
	Lines newpos;
	Vector3s2d newwidth1;
	Vector3s2d newwidth2;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		const LineEnd& le1 = les[i];
		if (le1.linkIdx1 != 0 && le1.linkIdx2 == 0)
		{
			if (!marked[le1.idx])
			{
				Line addline;
				Vector3s addwidth1;
				Vector3s addwidth2;
				const LineEnd* nowle = &le1;
				const LineEnd* last = 0;
				while (nowle)
				{
					marked[nowle->idx] = 1;
					const Line& loopline = pos[nowle->idx];
					const Vector3s& loopwidth1 = width.left[nowle->idx];
					const Vector3s& loopwidth2 = width.right[nowle->idx];
					const LineEnd* next = 0;
					if (last == 0)
					{
						next = nowle->linkIdx1;
					}
					else if (last == nowle->linkIdx1)
					{
						next = nowle->linkIdx2;
					}
					else
					{
						next = nowle->linkIdx1;
					}
					if (last == nowle->endlink)
					{
						addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
						addwidth1.insert(addwidth1.end(), loopwidth1.rbegin(), loopwidth1.rend());
						addwidth2.insert(addwidth2.end(), loopwidth2.rbegin(), loopwidth2.rend());
					}
					else if (last == nowle->beglink)
					{
						addline.insert(addline.end(), loopline.begin(), loopline.end());
						addwidth1.insert(addwidth1.end(), loopwidth1.begin(), loopwidth1.end());
						addwidth2.insert(addwidth2.end(), loopwidth2.begin(), loopwidth2.end());
					}
					else if (next == nowle->beglink)
					{
						addline.insert(addline.end(), loopline.rbegin(), loopline.rend());
						addwidth1.insert(addwidth1.end(), loopwidth1.rbegin(), loopwidth1.rend());
						addwidth2.insert(addwidth2.end(), loopwidth2.rbegin(), loopwidth2.rend());
					}
					else if (next == nowle->endlink)
					{
						addline.insert(addline.end(), loopline.begin(), loopline.end());
						addwidth1.insert(addwidth1.end(), loopwidth1.begin(), loopwidth1.end());
						addwidth2.insert(addwidth2.end(), loopwidth2.begin(), loopwidth2.end());
					}
					last = nowle;
					nowle = next;
				}
				newpos.push_back(addline);
				newwidth1.push_back(addwidth1);
				newwidth2.push_back(addwidth2);
			}
		}
	}
	for (int i = 0; i < (int)les.size(); ++i)
	{
		if (!marked[i])
		{
			newpos.push_back(pos[i]);
			newwidth1.push_back(width.left[i]);
			newwidth2.push_back(width.right[i]);
		}
	}
	pos = newpos;
	width.left = newwidth1;
	width.right = newwidth2;
}
Acutes LinkLineAcutes(LineEnds& les, double distance, double angle)
{
	Acutes res;
	double sdistance = distance * distance;
	for (int i = 0; i < (int)les.size(); ++i)
	{
		LineEnd& le1 = les[i];
		for (int j = 0; j < (int)les.size(); ++j)
		{
			if (i != j)
			{
				bool haslink = false;
				LineEnd& le2 = les[j];
				// find begin link
				if (le1.beg.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::BEGIN_TO_BEGIN, angle))
					{
						LineEndPoint lep;
						lep.dir = LineEndPoint::BEGIN;
						lep.le = le1;
						AddNewLineEndPoint(res, lep, sdistance);
						lep.dir = LineEndPoint::BEGIN;
						lep.le = le2;
						AddNewLineEndPoint(res, lep, sdistance);
						le1.beglink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.beg.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::BEGIN_TO_END, angle))
					{
						LineEndPoint lep;
						lep.dir = LineEndPoint::BEGIN;
						lep.le = le1;
						AddNewLineEndPoint(res, lep, sdistance);
						lep.dir = LineEndPoint::END;
						lep.le = le2;
						AddNewLineEndPoint(res, lep, sdistance);
						le1.beglink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				// find end link
				if (le1.end.squaredDistance(le2.beg) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::END_TO_BEGIN, angle))
					{
						LineEndPoint lep;
						lep.dir = LineEndPoint::END;
						lep.le = le1;
						AddNewLineEndPoint(res, lep, sdistance);
						lep.dir = LineEndPoint::BEGIN;
						lep.le = le2;
						AddNewLineEndPoint(res, lep, sdistance);
						le1.endlink = &le2;
						le2.beglink = &le1;
						haslink = true;
					}
				}
				else if (le1.end.squaredDistance(le2.end) < sdistance)
				{
					if (CheckLinkEnd_Similarity(le1, le2, LineEnd::END_TO_END, angle))
					{
						LineEndPoint lep;
						lep.dir = LineEndPoint::END;
						lep.le = le1;
						AddNewLineEndPoint(res, lep, sdistance);
						lep.dir = LineEndPoint::END;
						lep.le = le2;
						AddNewLineEndPoint(res, lep, sdistance);
						le1.endlink = &le2;
						le2.endlink = &le1;
						haslink = true;
					}
				}
				if (haslink)
				{
					if (le1.linkIdx1)
					{
						le1.linkIdx2 = &le2;
					}
					else
					{
						le1.linkIdx1 = &le2;
					}
					if (le2.linkIdx1)
					{
						le2.linkIdx2 = &le1;
					}
					else
					{
						le2.linkIdx1 = &le1;
					}
				}
			}
		}
	}
	return res;
}
double Distance(const Vector2& p, const Vector2& p1, const Vector2& p2)
{
	Vector2 v1 = p - p1, v2 = p2 - p1;
	return fabs(v1.crossProduct(v2)) / v2.length();
}
void MarkHasLink_LineEnds(LineEnds& les, const Lines& cvp)
{
	for (int i = 0; i < les.size(); ++i)
	{
		const Line& nline = cvp[les[i].idx];
		if (nline.size() > 3)
		{
			Vector2 v11 = nline[0];
			Vector2 v12 = nline[1];
			for (int j = 0; j < cvp.size(); ++j)
			{
				if (j != les[i].idx && !les[i].haslink1)
				{
					const Line& nline2 = cvp[j];
					for (int k = 0; k < nline2.size() - 1; ++k)
					{
						if (Distance(v11,  nline2[k], nline2[k + 1]) < 0.01)
						{
							printf(".");
							les[i].haslink1 = true;
							break;
						}
					}
				}
			}
			Vector2 v21 = nline[les[i].len];
			Vector2 v22 = nline[les[i].len - 1];
			for (int j = 0; j < cvp.size(); ++j)
			{
				if (j != les[i].idx && !les[i].haslink2)
				{
					const Line& nline2 = cvp[j];
					for (int k = 0; k < nline2.size() - 1; ++k)
					{
						if (Distance(v21,  nline2[k], nline2[k + 1]) < 0.01)
						{
							printf(".");
							les[i].haslink2 = true;
							break;
						}
					}
				}
			}
		}
	}
}
double cross(const Vector2& o, const Vector2& a, const Vector2& b)
{
	return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]);
}
bool intersect(const Vector2& p1, const Vector2& p2, const Vector2& p)
{
	return p[0] >= std::min(p1[0], p2[0])
		   && p[0] <= std::max(p1[0], p2[0])
		   && p[1] >= std::min(p1[1], p2[1])
		   && p[1] <= std::max(p1[1], p2[1]);
}
bool intersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2)
{
	double c1 = cross(a1, a2, b1);
	double c2 = cross(a1, a2, b2);
	double c3 = cross(b1, b2, a1);
	double c4 = cross(b1, b2, a2);
	if (c1 * c2 < 0 && c3 * c4 < 0)
	{
		return true;
	}
	if (abs(c1) < 0.001 && intersect(a1, a2, b1))
	{
		return true;
	}
	if (abs(c2) < 0.001 && intersect(a1, a2, b2))
	{
		return true;
	}
	if (abs(c3) < 0.001 && intersect(b1, b2, a1))
	{
		return true;
	}
	if (abs(c4) < 0.001 && intersect(b1, b2, a2))
	{
		return true;
	}
	return false;
}