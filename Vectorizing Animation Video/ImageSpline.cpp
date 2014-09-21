#define _USE_MATH_DEFINES
#include <cmath>
#include "ImageSpline.h"
#include "curve\HSplineCurve.h"
#include "curve\BezierCurve.h"
#include "curve\BsplineCurve.h"
#include <cassert>


ImageSpline::ImageSpline(void)
{
	m_w = 0;
	m_h = 0;
}

ImageSpline::~ImageSpline(void)
{
}

void ImageSpline::AddPatchSpline(const PatchLine& ps)
{
	m_PatchLines.push_back(ps);
}


void ImageSpline::ComputeToLineFragments()
{
	LineFragments& res = m_LineFragments;
	res.clear();
	for(int i = 0; i < m_PatchLines.size(); ++i)
	{
		for(int j = 0; j < m_PatchLines[i].m_LineFragments.size(); ++j)
		{
			bool alreadhas = false;
			Line& nowpoints = m_PatchLines[i].m_LineFragments[j].m_Points;
			if(nowpoints.empty()) { continue; }
			for(int k = 0; k < res.size(); ++k)
			{
				double len = (res[k].m_Points.front() - nowpoints.front()).length();
				if(len < 3)
				{
					bool issame = true;
					for(int i = 0; i < nowpoints.size() && i < res[k].m_Points.size(); ++i)
					{
						len = (res[k].m_Points[i] - nowpoints[i]).length();
						printf("%f\n", len);
						if(len > 3)
						{
							issame = false;
							break;
						}
					}
					if(issame)
					{
						if(nowpoints.size() > res[k].m_Points.size())
						{
							res[k].m_Points = nowpoints;
						}
						alreadhas = true;
						break;
					}
				}
				len = (res[k].m_Points.front() - nowpoints.back()).length();
				if(len < 3)
				{
					bool issame = true;
					const int nsize = (int)nowpoints.size();
					for(int i = 0; i < nowpoints.size() && i < res[k].m_Points.size(); ++i)
					{
						len = (res[k].m_Points[i] - nowpoints[nsize - i - 1]).length();
						printf("%f\n", len);
						if(len > 3)
						{
							issame = false;
							break;
						}
					}
					if(issame)
					{
						if(nowpoints.size() > res[k].m_Points.size())
						{
							res[k].m_Points = nowpoints;
						}
						alreadhas = true;
						break;
					}
				}
			}
			if(!alreadhas)
			{
				res.push_back(m_PatchLines[i].m_LineFragments[j]);
			}
		}
	}
	m_PatchSplines.clear();
}

SplineFragments& ImageSpline::GetSplineFragments()
{
	return m_SplineFragments;
}

void ImageSpline::SmoothingFragments()
{
	m_SplineFragments.clear();
	// 平均各點
	for(int count = 0; count < 4; count++)
	{
		for(int i = 0; i < m_LineFragments.size(); ++i)
		{
			Line& cps = m_LineFragments[i].m_Points;
			Line newcps;
			int last = (int)cps.size() - 1;
			if(cps.size() < 5) { continue; }
			newcps.push_back(cps.front());
			newcps.push_back((cps[0] + cps[1] * 2 + cps[2] + cps[3]) / 5.0f);
			for(int j = 2; j < cps.size() - 2; j ++)
			{
//               if (CheckOnSide((int)cps[j].x, (int)cps[j].y))
//               {
//                   newcps.push_back(cps[j]);
//               }
//               else
				{
					auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
							   6.0f;
					newcps.push_back(vec);
				}
			}
			newcps.push_back((cps[last - 3] + cps[last - 2] + cps[last - 1] * 2 +
							  cps[last]) / 5.0f);
			newcps.push_back(cps.back());
			cps = newcps;
		}
	}
	m_Controls.resize(m_LineFragments.size());
	for(int i = 0; i < m_LineFragments.size(); ++i)
	{
		//HSplineCurve hs;
		BezierCurve hs;
		Line& cps = m_Controls[i];
		Line& res = m_LineFragments[i].m_Points;
		Vector2 beg = res.front(), end = res.back();
		for(int j = 0; j < res.size(); j += 1)
		{
			hs.AddPointByDistance(res[j]);
		}
		double dis = hs.GetDistance();
		int step = dis / 3.0;
		cps.push_back(beg);
		for(int j = 1; j < step; ++j)
		{
			cps.push_back(hs.GetValue(j * 3));
		}
		cps.push_back(end);
//      hs.Clear();
//      for(int j = 0; j < cps.size(); ++j)
//      {
//          hs.AddPointByDistance(cps[j]);
//      }
		res.clear();
		res.push_back(beg);
		for(int j = 1; j < dis - 1; ++j)
		{
			res.push_back(hs.GetValue(j));
		}
		res.push_back(end);
	}
	// 略過 只折一次點
//  for (int i = 0; i < m_LineFragments.size(); ++i)
//  {
//      Line& cps = m_LineFragments[i].m_Points;
//      Line newcps;
//
//      if (cps.size() < 5) { continue; }
//
//      newcps.push_back(cps.front());
//      Vector2 last(cps[1].x - cps[0].x , cps[1].y - cps[0].y);
//      int bigger0 = 0, smaller0 = 0;
//      int corner = 0;
//
//      for (int j = 1; j < cps.size() - 1; ++j)
//      {
//          Vector2 vec = cps[j + 1] - cps[j];
//          corner++;
//
//          if (last != vec)
//          {
//              float angle1 = atan2(last.x, last.y) * M_1_PI * 180;
//              float angle2 = atan2(vec.x, vec.y) * M_1_PI * 180;
//
//              if (angle2 - angle1 > 0)
//              {
//                  bigger0++;
//                  smaller0 = 0;
//              }
//              else if (angle2 - angle1 < 0)
//              {
//                  bigger0 = 0;
//                  smaller0++;
//              }
//
//              if (bigger0 > 1)
//              {
//                  if (cps.back() != cps[j])
//                  {
//                      newcps.push_back(cps[j]);
//                  }
//
//                  corner = 0;
//              }
//              else if (smaller0 > 0)
//              {
//                  if (cps.back() != cps[j])
//                  {
//                      newcps.push_back(cps[j]);
//                  }
//
//                  corner = 0;
//              }
//              else if (corner > 3)
//              {
//                  if (cps.back() != cps[j])
//                  {
//                      newcps.push_back(cps[j]);
//                  }
//
//                  corner = 0;
//              }
//
//              last = vec;
//          }
//          else
//          {
//              if (corner > 10)
//              {
//                  if (cps.back() != cps[j])
//                  {
//                      newcps.push_back(cps[j]);
//                  }
//
//                  corner = 0;
//              }
//          }
//      }
//
//      if (cps.back() != newcps.back())
//      {
//          newcps.push_back(cps.back());
//      }
//
//      cps = newcps;
//  }
	// 內插出中間點
//  for (int i = 0; i < m_LineFragments.size(); ++i)
//  {
//      BezierCurve hc;
//      Line& cps = m_LineFragments[i].m_Points;
//      Line newcps;
//
//      if (cps.size() < 5) { continue; }
//
//      float sum = 0;
//      hc.AddPoint(0 , cps.front());
//
//      for (int j = 1; j < cps.size(); ++j)
//      {
//          Vector2 vec = cps[j] - cps[j - 1];
//          sum += vec.length();
//          hc.AddPoint(sum , cps[j]);
//      }
//
//      float step = sum / (cps.size() * 2);
//
//      for (float j = 0; j < sum-step; j += step)
//      {
//          Vector2 v = hc.GetValue(j);
//          newcps.push_back(v);
//      }
//      newcps.push_back(cps.back());
//      cps = newcps;
//  }
//  m_ControlPoints = m_LineFragments;
//  for (int i = 0; i < m_LineFragments.size(); ++i)
//  {
//      m_SplineFragments.push_back(SplineFragment());
//      SplineFragment& sf = m_SplineFragments.back();
//      Line& cps = m_LineFragments[i].m_Points;
//
//      for (int j = 0; j < cps.size(); ++j)
//      {
//          sf.AddPoint(Vector2(cps[j].x, cps[j].y));
//      }
//
//      sf.CurveFitting(0.3);
//      int newsize = cps.size();
//
//      newsize *= 0.3;
//
//      float step = 1.0 / (newsize);
//      cps.clear();
//
//      for (int j = 0; j < newsize; ++j)
//      {
//          Vector2 v = sf.GetVector2(step * j);
//
//          if (j > 0 && cps.back() != v)
//          {
//              cps.push_back(v);
//          }
//      }
//  }
}

void ImageSpline::SetSize(int w, int h)
{
	m_w = w;
	m_h = h;
}

bool ImageSpline::CheckOnSide(int x, int y)
{
	if(x == 0 || y == 0)
	{
		return true;
	}
	assert(m_w != 0);
	if(x == m_w || y == m_h)
	{
		return true;
	}
	return false;
}

void ImageSpline::RemoveShadingLine(vavImage& vimg)
{
	for(int i = 0; i < m_LineFragments.size(); ++i)
	{
		Line& cps = m_LineFragments[i].m_Points;
		int isBlack = 0;
		for(int j = 0; j < cps.size(); j ++)
		{
			if(vimg.IsBlackLine(cps[j].x, cps[j].y, 6))
			{
				isBlack++;
			}
		}
		printf("isBlack: %d now_line.size(): %d\n", isBlack, cps.size());
		if(isBlack < cps.size() / 2)
		{
			m_LineFragments.erase(m_LineFragments.begin() + i);
			i--;
		}
	}
}
