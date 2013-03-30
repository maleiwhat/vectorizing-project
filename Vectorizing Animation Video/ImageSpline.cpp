#define _USE_MATH_DEFINES
#include <cmath>
#include "ImageSpline.h"
#include "curve\HSplineCurve.h"
#include "curve\BezierCurve.h"
#include "curve\BsplineCurve.h"
#include <cassert>


ImageSpline::ImageSpline(void)
{
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

	for (int i = 0; i < m_PatchLines.size(); ++i)
	{
		for (int j = 0; j < m_PatchLines[i].m_LineFragments.size(); ++j)
		{
			bool alreadhas = false;
			Line& nowpoints = m_PatchLines[i].m_LineFragments[j].m_Points;

			if (nowpoints.empty()) { continue; }

			for (int k = 0; k < res.size(); ++k)
			{
				double len = (res[k].m_Points.front() - nowpoints.front()).length();

				if (len < 3)
				{
					bool issame = true;

					for (int i = 0; i < nowpoints.size() && i < res[k].m_Points.size(); ++i)
					{
						len = (res[k].m_Points[i] - nowpoints[i]).length();
						printf("%f\n", len);

						if (len > 3)
						{
							issame = false;
							break;
						}
					}

					if (issame)
					{
						if (nowpoints.size() > res[k].m_Points.size())
						{
							res[k].m_Points = nowpoints;
						}

						alreadhas = true;
						break;
					}
				}

				len = (res[k].m_Points.front() - nowpoints.back()).length();

				if (len < 3)
				{
					bool issame = true;
					const int nsize = nowpoints.size();

					for (int i = 0; i < nowpoints.size() && i < res[k].m_Points.size(); ++i)
					{
						len = (res[k].m_Points[i] - nowpoints[nsize - i - 1]).length();
						printf("%f\n", len);

						if (len > 3)
						{
							issame = false;
							break;
						}
					}

					if (issame)
					{
						if (nowpoints.size() > res[k].m_Points.size())
						{
							res[k].m_Points = nowpoints;
						}

						alreadhas = true;
						break;
					}
				}
			}

			if (!alreadhas)
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

void ImageSpline::ComputeToSplineFragments()
{
	// 還原圖的大小
	for (int i = 0; i < m_LineFragments.size(); ++i)
	{
		Line& cps = m_LineFragments[i].m_Points;
		Line newcps;

		for (int j = 0; j < cps.size(); j ++)
		{
			if (int(cps[j].x) % 2 == 0 && int(cps[j].y) % 2 == 0)
			{
				newcps.push_back(cps[j] / 2);
			}
		}

		cps = newcps;
	}

	return;
	m_SplineFragments.clear();

	// 平均各點
// 	for (int count = 0; count < 7; count++)
// 	{
// 		for (int i = 0; i < m_LineFragments.size(); ++i)
// 		{
// 			Line& cps = m_LineFragments[i].m_Points;
// 			Line newcps;
// 
// 			if (cps.size() < 4) { continue; }
// 
// 			newcps.push_back(cps.front());
// 
// 			for (int j = 1; j < cps.size() - 1; j ++)
// 			{
// 				Vector2 vec = (cps[j] * 3 + cps[j + 1] * 0.5 + cps[j - 1] * 0.5) / 4.0f;
// 				newcps.push_back(vec);
// 			}
// 
// 			if (cps.back() != newcps.back())
// 			{
// 				newcps.push_back(cps.back());
// 			}
// 
// 			cps = newcps;
// 		}
// 	}

	
	// 略過 只折一次點

// 	for (int i = 0; i < m_LineFragments.size(); ++i)
// 	{
// 		Line& cps = m_LineFragments[i].m_Points;
// 		Line newcps;
// 
// 		if (cps.size() < 5) { continue; }
// 
// 		newcps.push_back(cps.front());
// 		Vector2 last(cps[1].x - cps[0].x , cps[1].y - cps[0].y);
// 		int bigger0 = 0, smaller0 = 0;
// 		int corner = 0;
// 
// 		for (int j = 1; j < cps.size() - 1; ++j)
// 		{
// 			Vector2 vec = cps[j + 1] - cps[j];
// 			corner++;
// 
// 			if (last != vec)
// 			{
// 				float angle1 = atan2f(last.x, last.y) / M_PI * 180;
// 				float angle2 = atan2f(vec.x, vec.y) / M_PI * 180;
// 
// 				if (angle2 - angle1 > 0)
// 				{
// 					bigger0++;
// 					smaller0 = 0;
// 				}
// 				else if (angle2 - angle1 < 0)
// 				{
// 					bigger0 = 0;
// 					smaller0++;
// 				}
// 
// 				if (bigger0 > 1)
// 				{
// 					if (cps.back() != cps[j])
// 					{
// 						newcps.push_back(cps[j]);
// 					}
// 
// 					corner = 0;
// 				}
// 				else if (smaller0 > 0)
// 				{
// 					if (cps.back() != cps[j])
// 					{
// 						newcps.push_back(cps[j]);
// 					}
// 
// 					corner = 0;
// 				}
// 				else if (corner > 3)
// 				{
// 					if (cps.back() != cps[j])
// 					{
// 						newcps.push_back(cps[j]);
// 					}
// 
// 					corner = 0;
// 				}
// 
// 				last = vec;
// 			}
// 			else
// 			{
// 				if (corner > 10)
// 				{
// 					if (cps.back() != cps[j])
// 					{
// 						newcps.push_back(cps[j]);
// 					}
// 
// 					corner = 0;
// 				}
// 			}
// 		}
// 
// 		if (cps.back() != newcps.back())
// 		{
// 			newcps.push_back(cps.back());
// 		}
// 
// 		cps = newcps;
// 	}
//	return ;
	
	// 內插出中間點
// 	for (int i = 0; i < m_LineFragments.size(); ++i)
// 	{
// 		BezierCurve hc;
// 		Line& cps = m_LineFragments[i].m_Points;
// 		Line newcps;
//
// 		if (cps.size() < 5) { continue; }
//
// 		float sum = 0;
// 		hc.AddPoint(0 , cps.front());
//
// 		for (int j = 1; j < cps.size(); ++j)
// 		{
// 			Vector2 vec = cps[j] - cps[j - 1];
// 			sum += vec.length();
// 			hc.AddPoint(sum , cps[j]);
// 		}
//
// 		float step = sum / (cps.size() * 2);
//
// 		for (float j = 0; j < sum-step; j += step)
// 		{
// 			Vector2 v = hc.GetValue(j);
// 			newcps.push_back(v);
// 		}
// 		newcps.push_back(cps.back());
// 		cps = newcps;
// 	}
// 	m_ControlPoints = m_LineFragments;
// 	for (int i = 0; i < m_LineFragments.size(); ++i)
// 	{
// 		m_SplineFragments.push_back(SplineFragment());
// 		SplineFragment& sf = m_SplineFragments.back();
// 		Line& cps = m_LineFragments[i].m_Points;
//
// 		for (int j = 0; j < cps.size(); ++j)
// 		{
// 			sf.AddPoint(Vector2(cps[j].x, cps[j].y));
// 		}
//
// 		sf.CurveFitting(0.3);
// 		int newsize = cps.size();
//
// 		newsize *= 0.3;
//
// 		float step = 1.0 / (newsize);
// 		cps.clear();
//
// 		for (int j = 0; j < newsize; ++j)
// 		{
// 			Vector2 v = sf.GetVector2(step * j);
//
// 			if (j > 0 && cps.back() != v)
// 			{
// 				cps.push_back(v);
// 			}
// 		}
// 	}
}
