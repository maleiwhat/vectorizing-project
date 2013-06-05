#include "Patch.h"
#include "SplineShape.h"
#include "algSpline.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"
#include "math\Quaternion.h"

void Patch::AddOuterPoint(double x, double y)
{
	m_Outer.push_back(Vector2(x, y));
}

Vector2s2d& Patch::Inter()
{
	return m_Inter;
}

void Patch::Clear()
{
	m_Outer.clear();
	m_Inter.clear();
}

Vector2s& Patch::Outer()
{
	return m_Outer;
}

void Patch::SplinePoints(double scale)
{
	if (m_Outer.size() > 5)
	{
		SplineShape ss;
		algSpline sr, sg, sb;

		for (auto it = m_Outer.begin(); it != m_Outer.end(); ++it)
		{
			ss.AddPoint(it->x, it->y);
		}

		for (auto it = m_OuterColor.begin(); it != m_OuterColor.end(); ++it)
		{
			sr.AddPoint(it->x);
			sg.AddPoint(it->y);
			sb.AddPoint(it->z);
		}

		int new_size = m_Outer.size() * scale;
		float   step = 1.f /  new_size;
		m_Outer.clear();
		m_OuterColor.clear();

		for (int i = 0; i <= new_size; ++i)
		{
			float t = step * i;
			Vector2 v = ss.GetPoint(t);
			m_Outer.push_back(Vector2(v.x, v.y));
		}
	}

	int j = 0;

	for (auto it = m_Inter.begin(); it != m_Inter.end(); ++it, ++j)
	{
		SplineShape ss;
		algSpline sr, sg, sb;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			ss.AddPoint(it2->x, it2->y);
		}

		int new_size = it->size() * scale;

		if (new_size < 5) { continue; }

		float step = 1.f / (new_size);
		it->clear();

		for (int i = 0; i <= new_size; ++i)
		{
			float t = step * i;
			Vector2 v = ss.GetPoint(t);
			it->push_back(Vec2(v.x, v.y));
		}
	}
}

Vector3s& Patch::OuterColor()
{
	return m_OuterColor;
}

Vector3s2d& Patch::InterColor()
{
	return m_InterColor;
}

void Patch::Deflate(double len)
{
	Vector2s normals;
	normals.push_back(Quaternion::GetRotation(m_Outer[1] - m_Outer.back(), -90));

	for (int i = 1; i < m_Outer.size() - 1 ; ++i)
	{
		normals.push_back(Quaternion::GetRotation(m_Outer[i + 1] - m_Outer[i - 1],
		                  -90));
	}

	normals.push_back(Quaternion::GetRotation(m_Outer[0] -
	                  m_Outer[m_Outer.size() - 2], -90));

	for (int i = 0; i < normals.size() ; ++i)
	{
		normals[i].normalise();
		normals[i] *= len;
	}

	for (int i = 0; i < normals.size() ; ++i)
	{
		m_Outer[i] += normals[i];
	}
}

void Patch::Inflate(double len)
{
	Vector2s normals;
	normals.push_back(m_Outer[1] - m_Outer.back());

	for (int i = 1; i < m_Outer.size() - 1 ; ++i)
	{
		normals.push_back(m_Outer[i + 1] - m_Outer[i - 1]);
	}

	normals.push_back(m_Outer[0] - m_Outer[m_Outer.size() - 2]);

	for (int i = 0; i < normals.size() ; ++i)
	{
		normals[i].normalise();
		normals[i] *= len;
	}

	for (int i = 0; i < normals.size() ; ++i)
	{
		m_Outer[i] -= normals[i];
	}
}

ColorConstraint_sptr Patch::GetColorConstraint()
{
	ColorConstraint_sptr res = ColorConstraint_sptr(new ColorConstraint);

	for (int i = 0; i < m_Outer.size(); ++i)
	{
		res->AddPoint(m_Outer[i].x, m_Outer[i].y, m_OuterColor[i]);
	}

	for (int i = 0; i < m_Inter.size(); ++i)
	{
		for (int j = 0; j < m_Inter[i].size(); ++j)
		{
			res->AddPoint(m_Inter[i][j].x, m_Inter[i][j].y, m_InterColor[i][j]);
		}
	}

	return res;
}

void Patch::SmoothPatch()
{
	for (int count = 0; count < 4; count++)
	{
		Vector2s& cps = m_Outer;
		Vector2s newcps;
		int last = cps.size() - 1;

		if (cps.size() < 5) { continue; }

		newcps.push_back((cps[last - 1] + cps[last] + cps[0] * 2 + cps[1] + cps[2]) /
		                 6.0f);
		newcps.push_back((cps[last] + cps[0] + cps[1] * 2 + cps[2] + cps[3]) / 6.0f);

		for (int j = 2; j < cps.size() - 2; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
			           6.0f;
			newcps.push_back(vec);
		}

		newcps.push_back((cps[last - 3] + cps[last - 2] + cps[last - 1] * 2 + cps[last]
		                  + cps[0]) / 6.0f);
		newcps.push_back((cps[last - 2] + cps[last - 1] + cps[last] * 2 + cps[0] +
		                  cps[1]) / 6.0f);
		cps = newcps;
	}

	for (int count = 0; count < 4; count++)
	{
		for (int i = 0; i < m_Inter.size(); ++i)
		{
			Vector2s& cps = m_Inter[i];
			Vector2s newcps;
			int last = cps.size() - 1;

			if (cps.size() < 5) { continue; }

			newcps.push_back((cps[last - 1] + cps[last] + cps[0] * 2 + cps[1] + cps[2]) /
			                 6.0f);
			newcps.push_back((cps[last] + cps[0] + cps[1] * 2 + cps[2] + cps[3]) / 6.0f);

			for (int j = 2; j < cps.size() - 2; j ++)
			{
				auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
				           6.0f;
				newcps.push_back(vec);
			}

			newcps.push_back((cps[last - 3] + cps[last - 2] + cps[last - 1] * 2 + cps[last]
			                  + cps[0]) / 6.0f);
			newcps.push_back((cps[last - 2] + cps[last - 1] + cps[last] * 2 + cps[0] +
			                  cps[1]) / 6.0f);
			cps = newcps;
		}
	}
}

void CvPatch::AddOuterPoint(double x, double y)
{
	m_Outer.push_back(cv::Point(x, y));
}

CvPoints2d& CvPatch::Inter()
{
	return m_Inter;
}

void CvPatch::Clear()
{
	m_Outer.clear();
	m_Inter.clear();
}

CvPoints& CvPatch::Outer()
{
	return m_Outer;
}

void CvPatch::SetImage(cv::Mat& image)
{
	m_refImage = &image;
	m_OuterColor.clear();
	m_InterColor.clear();

	for (auto it = m_Outer2.begin(); it != m_Outer2.end(); ++it)
	{
		cv::Vec3b& c = GetColor(image, it->x / 2, it->y / 2);
		m_OuterColor.push_back(c);
	}

	for (auto it = m_Inter2.begin(); it != m_Inter2.end(); ++it)
	{
		Vec3bs data;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			cv::Vec3b& c = GetColor(image, it2->x / 2, it2->y / 2);
			data.push_back(c);
		}

		m_InterColor.push_back(data);
	}
}

Vec3bs& CvPatch::OuterColor()
{
	return m_OuterColor;
}

Vec3bs2d& CvPatch::InterColor()
{
	return m_InterColor;
}

ColorConstraint_sptr CvPatch::GetColorConstraint()
{
	ColorConstraint_sptr res = ColorConstraint_sptr(new ColorConstraint);
	cv::Rect rect = cv::boundingRect(m_Outer2);

	for (int i = rect.x; i < rect.x + rect.width - 1; i += 3)
	{
		for (int j = rect.y; j < rect.y + rect.height - 1; j += 3)
		{
			if (Inside(i, j))
			{
				res->AddPoint(i / 2, j / 2, m_refImage->at<cv::Vec3b>(j / 2, i / 2));
			}
		}
	}

//  for (int i = 0; i < m_Outer2.size(); ++i)
//  {
//      res->AddPoint(m_Outer2[i].x, m_Outer2[i].y, m_OuterColor[i]);
//  }
//
//  for (int i = 0; i < m_Inter2.size(); ++i)
//  {
//      for (int j = 0; j < m_Inter2[i].size(); ++j)
//      {
//          res->AddPoint(m_Inter2[i][j].x, m_Inter2[i][j].y, m_InterColor[i][j]);
//      }
//  }
	return res;
}

CvPoints& CvPatch::Outer2()
{
	return m_Outer2;
}

CvPoints2d& CvPatch::Inter2()
{
	return m_Inter2;
}

bool CvPatch::Inside(double x, double y)
{
	if (cv::pointPolygonTest(m_Outer2, cv::Point2f(x, y), false) > 0)
	{
		for (int i = 0; i < m_Inter.size(); ++i)
		{
			if (cv::pointPolygonTest(m_Inter[i], cv::Point2f(x, y), false) > 0)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

Patch ToPatch(CvPatch& cvp)
{
	Patch p;

	for (auto it = cvp.Outer2().begin(); it != cvp.Outer2().end(); ++it)
	{
		p.Outer().push_back(Vector2(it->x, it->y));
	}

	for (auto it = cvp.OuterColor().begin(); it != cvp.OuterColor().end(); ++it)
	{
		p.OuterColor().push_back(Vector3((*it)[0], (*it)[1], (*it)[2]));
	}

	for (auto it = cvp.Inter2().begin(); it != cvp.Inter2().end(); ++it)
	{
		Vector2s data;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			data.push_back(Vector2(it2->x, it2->y));
		}

		p.Inter().push_back(data);
	}

	for (auto it = cvp.InterColor().begin(); it != cvp.InterColor().end(); ++it)
	{
		Vector3s data;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			data.push_back(Vector3((*it2)[0], (*it2)[1], (*it2)[2]));
		}

		p.InterColor().push_back(data);
	}

	return p;
}

CvPatch ToCvPatch(Patch& cvp)
{
	CvPatch p;

	for (auto it = cvp.Outer().begin(); it != cvp.Outer().end(); ++it)
	{
		p.Outer().push_back(cv::Point(it->x, it->y));
	}

	for (auto it = cvp.OuterColor().begin(); it != cvp.OuterColor().end(); ++it)
	{
		p.OuterColor().push_back(cv::Vec3b(it->x, it->y, it->z));
	}

	for (auto it = cvp.Inter().begin(); it != cvp.Inter().end(); ++it)
	{
		CvPoints data;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			data.push_back(cv::Point(it2->x, it2->y));
		}

		p.Inter().push_back(data);
	}

	for (auto it = cvp.InterColor().begin(); it != cvp.InterColor().end(); ++it)
	{
		Vec3bs data;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			data.push_back(cv::Vec3b(it2->x, it2->y, it2->z));
		}

		p.InterColor().push_back(data);
	}

	return p;
}
