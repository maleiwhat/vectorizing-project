#include "stdcv.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include "vavImage.h"
#include "CvExtenstion2.h"
#include "math/Quaternion.h"
#include "Line.h"
#include "IFExtenstion.h"

ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask,
								 const cv::Mat& img)
{
	ColorConstraint_sptrs ans;
	for (int i = 0; i < regions; ++i)
	{
		ans.push_back(ColorConstraint_sptr(new ColorConstraint));
	}
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			const cv::Vec3b& id_color = mask.at<cv::Vec3b>(i, j);
			int idx = id_color[0] + id_color[1] * 255 + id_color[2] * 255 * 255;
			const cv::Vec3b& color = img.at<cv::Vec3b>(i, j);
			ans[idx % ans.size()]->AddPoint(j, i, Vector3(color[0], color[1], color[2]));
		}
	}
	return ans;
}

cv::Vec3b& GetColor(cv::Mat& image, int x, int y)
{
	if (y < 0)
	{
		y = 0;
	}
	if (y >= image.rows)
	{
		y = image.rows - 1;
	}
	if (x < 0)
	{
		x = 0;
	}
	if (x >= image.cols)
	{
		x = image.cols - 1;
	}
	return image.at<cv::Vec3b>(y, x);
}

Vector3s2d GetLinesColor(cv::Mat img, const Lines& lines)
{
	vavImage vimg(img);
	Vector3s2d ans;
	for (auto it = lines.cbegin(); it != lines.cend(); ++it)
	{
		ans.push_back(Vector3s());
		Vector3s& li = ans.back();
		for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2)
		{
			double r = vimg.GetBilinearR_if0(it2->x, it2->y);
			double g = vimg.GetBilinearG_if0(it2->x, it2->y);
			double b = vimg.GetBilinearB_if0(it2->x, it2->y);
			li.push_back(Vector3(r, g, b));
		}
	}
	return ans;
}

Vector3s GetLinesColor(cv::Mat img, const Line& lines)
{
	vavImage vimg(img);
	Vector3s ans;
	for (auto it2 = lines.cbegin(); it2 != lines.cend(); ++it2)
	{
		double r = vimg.GetBilinearR_if0(it2->x, it2->y);
		double g = vimg.GetBilinearG_if0(it2->x, it2->y);
		double b = vimg.GetBilinearB_if0(it2->x, it2->y);
		ans.push_back(Vector3(r, g, b));
	}
	return ans;
}

Color2Side GetLinesColor2Side(cv::Mat img, const Lines& lines, double normal_len)
{
	vavImage vimg(img);
	Vector2s2d normals(lines.size());
	for (int i = 0; i < lines.size() ; ++i)
	{
		const Line& now_line = lines[i];
		normals[i].resize(now_line.size());
		for (int j = 0; j < now_line.size() - 1 ; ++j)
		{
			normals[i][j] = Quaternion::GetRotation(now_line[j + 1] - now_line[j],
													-90);
		}
		normals[i].back() = Quaternion::GetRotation(now_line[now_line.size() - 1] -
							now_line[now_line.size() - 2], -90);
	}
	for (int i = 0; i < normals.size() ; ++i)
	{
		for (int j = 0; j < normals[i].size() ; ++j)
		{
			normals[i][j].normalise();
		}
	}
	Color2Side ans;
	Vector3s2d& ans_left = ans.left;
	Vector3s2d& ans_right = ans.right;
	ans_left.resize(lines.size());
	ans_right.resize(lines.size());
	for (int i = 0; i < lines.size(); ++i)
	{
		Lines::const_iterator it = lines.cbegin() + i;
		Vector3s& li = ans_left[i];
		int j = 0;
		li.resize(it->size());
		for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
		{
			Vector2 pos = *it2 - normals[i][j] * normal_len;
			double r = vimg.GetBilinearR_if0(pos.x, pos.y);
			double g = vimg.GetBilinearG_if0(pos.x, pos.y);
			double b = vimg.GetBilinearB_if0(pos.x, pos.y);
			li[j] = Vector3(r, g, b);
		}
		Vector3s& ri = ans_right[i];
		ri.resize(it->size());
		j = 0;
		for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
		{
			Vector2 pos = *it2 + normals[i][j] * normal_len;
			double r = vimg.GetBilinearR_if0(pos.x, pos.y);
			double g = vimg.GetBilinearG_if0(pos.x, pos.y);
			double b = vimg.GetBilinearB_if0(pos.x, pos.y);
			ri[j] = Vector3(r, g, b);
		}
	}
	return ans;
}

Color2Side GetLinesColor2SideSmart(cv::Mat img, const Lines& lines)
{
	vavImage vimg(img);
	Lines showLines;
	Lines BLineWidth(lines.size());
	Vector2s2d normals(lines.size());
	for (int i = 0; i < lines.size() ; ++i)
	{
		const Line& now_line = lines[i];
		normals[i].resize(now_line.size());
		for (int j = 0; j < now_line.size() - 1 ; ++j)
		{
			normals[i][j] = Quaternion::GetRotation(now_line[j + 1] - now_line[j],
													-90);
		}
		normals[i].back() = Quaternion::GetRotation(now_line[now_line.size() - 1] -
							now_line[now_line.size() - 2], -90);
	}
	for (int i = 0; i < normals.size() ; ++i)
	{
		for (int j = 0; j < normals[i].size() ; ++j)
		{
			normals[i][j].normalise();
		}
	}
	normals = SmoothingLen5(normals, 0, 3);
	const double blackRadio = 0.7;
	for (int idx1 = 0; idx1 < lines.size(); ++idx1)
	{
		const Line& nowLine = lines[idx1];
		const Line& nowNormals = normals[idx1];
		Line& lineWidths = BLineWidth[idx1];
		lineWidths.clear();
		for (int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
		{
			const double LINE_WIDTH = 4;
			Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
			Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
			Vector2 start2(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * LINE_WIDTH);
			Vector2 end2(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * LINE_WIDTH);
			double_vector line1 = vimg.GetLineLight(start.x, start.y, end.x, end.y,
													360);
			double_vector line2 = vimg.GetLineLight(start2.x, start2.y, end2.x, end2.y,
													360);
			line1 = SmoothingLen5(line1, 0.0, 3);
			line2 = SmoothingLen5(line2, 0.0, 3);
			double_vector width1 = GetColorWidth(ConvertToSquareWave(ConvertToAngle(line1),
												15, 50), LINE_WIDTH * 2);
			double_vector width2 = GetColorWidth(ConvertToSquareWave(ConvertToAngle(line2),
												15, 50), LINE_WIDTH * 2);
			if (width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1)
			{
				Line line1;
				line1.push_back(nowLine[idx2] - nowNormals[idx2] * width1[0] * blackRadio);
				line1.push_back(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * width2[0] *
								blackRadio);
				line1 = GetLine(line1, 0.5, 0.5);
				Line line2;
				line2.push_back(nowLine[idx2] + nowNormals[idx2] * width1[1] * blackRadio);
				line2.push_back(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * width2[1] *
								blackRadio);
				line2 = GetLine(line2, 0.5, 0.5);
				showLines.push_back(line1);
				showLines.push_back(line2);
				// save line width
				lineWidths.push_back(Vector2(width1[0] * blackRadio, width1[1] * blackRadio));
			}
			else
			{
				lineWidths.push_back(Vector2());
			}
		}
		lineWidths.push_back(Vector2());
	}
	Color2Side ans;
	Vector3s2d& ans_left = ans.left;
	Vector3s2d& ans_right = ans.right;
	ans_left.resize(lines.size());
	ans_right.resize(lines.size());
	for (int i = 0; i < lines.size(); ++i)
	{
		Lines::const_iterator it = lines.cbegin() + i;
		Vector3s& li = ans_left[i];
		Line& lineWidths = BLineWidth[i];
		int j = 0;
		li.resize(it->size());
		for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
		{
			if (lineWidths[j][0] > 0)
			{
				Vector2 pos = *it2 - normals[i][j] * (lineWidths[j][0]+0.5);
				double r = vimg.GetBilinearR_if0(pos.x, pos.y);
				double g = vimg.GetBilinearG_if0(pos.x, pos.y);
				double b = vimg.GetBilinearB_if0(pos.x, pos.y);
				li[j] = Vector3(r, g, b);
			}
		}
		Vector3s& ri = ans_right[i];
		ri.resize(it->size());
		j = 0;
		for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
		{
			if (lineWidths[j][1] > 0)
			{
				Vector2 pos = *it2 + normals[i][j] * (lineWidths[j][1]+0.5);
				double r = vimg.GetBilinearR_if0(pos.x, pos.y);
				double g = vimg.GetBilinearG_if0(pos.x, pos.y);
				double b = vimg.GetBilinearB_if0(pos.x, pos.y);
				ri[j] = Vector3(r, g, b);
			}
		}
	}
	return ans;
}

void OutputDiffusionCurve(std::string name, int w, int h, Color2Side& c2s,
						  Lines& lines)
{
	std::ofstream out(name);
	out << "<!DOCTYPE CurveSetXML>" << std::endl;
	out << " <curve_set image_width=\"" << w << "\" image_height=\"" << h <<
		"\" nb_curves=\"" << lines.size() << "\" >" << std::endl;
	for (int i = 0; i < lines.size(); ++i)
	{
		Line& now_line = lines[i];
		Vector3s& now_left = c2s.left[i];
		Vector3s& now_right = c2s.right[i];
		for (int j = 0; j < now_line.size(); j ++)
		{
			now_line[j] = Quaternion::GetRotation(now_line[j], 90);
			now_line[j].y = -now_line[j].y;
		}
//      if (now_left.size() > 5000)
//      {
//          Vector3s tmp;
//
//          for (int j = 0; j < now_left.size(); j += 2)
//          {
//              tmp.push_back(now_left[j]);
//          }
//
//          now_left = tmp;
//          tmp.clear();
//
//          for (int j = 0; j < now_right.size(); j += 2)
//          {
//              tmp.push_back(now_right[j]);
//          }
//
//          now_right = tmp;
//      }
		out << " <curve nb_control_points=\"" << now_line.size() <<
			"\" nb_left_colors=\"" << now_left.size() << "\" nb_right_colors=\"" <<
			now_right.size() << "\" nb_blur_points=\"2\" lifetime=\"32\" >" <<
			std::endl;
		out << "  <control_points_set>" << std::endl;
		for (int j = 0; j < now_line.size(); j++)
		{
			out << "   <control_point x=\"" << now_line[j].x << "\" y=\"" << now_line[j].y
				<< "\" />" << std::endl;
		}
		out << "  </control_points_set>" << std::endl;
		out << "  <left_colors_set>" << std::endl;
		double globalID_step = 10000.0 / (now_left.size() - 1);
		for (int j = 0; j < now_left.size(); j++)
		{
			out << "   <left_color G=\"" << (int)now_left[j].y << "\" R=\"" <<
				(int)now_left[j].z << "\" globalID=\"" << (int)(j * globalID_step) <<
				"\" B=\"" << (int)now_left[j].x << "\" />" << std::endl;
		}
		out << "  </left_colors_set>" << std::endl;
		out << "  <right_colors_set>" << std::endl;
		for (int j = 0; j < now_right.size() ; j++)
		{
			out << "   <right_color G=\"" << (int)now_right[j].y << "\" R=\"" <<
				(int)now_right[j].z << "\" globalID=\"" << (int)(j * globalID_step) <<
				"\" B=\"" << (int)now_right[j].x << "\" />" << std::endl;
		}
		out << "  </right_colors_set>" << std::endl;
		out << "  <blur_points_set>" << std::endl;
		out << "   <best_scale value=\"0\" globalID=\"0\" />" << std::endl;
		out << "   <best_scale value=\"0\" globalID=\"10000\" />" << std::endl;
		out << "  </blur_points_set>" << std::endl;
		out << " </curve>" << std::endl;
	}
	out << "</curve_set>" << std::endl;
}

cv::Mat MakeIsoSurfaceImg(cv::Mat img, int n)
{
	//cv::GaussianBlur(img, img, cv::Size(5, 5), 2);
	//cv::GaussianBlur(img, img, cv::Size(3, 3), 1);
	cv::Mat ans = img.clone();
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			cv::Vec3b& v = img.at<cv::Vec3b>(i, j);
			float vv = 0.299 * v[2] + 0.587 * v[1] + 0.114 * v[0];
			vv = int(vv / n) * n;
			cv::Vec3b& a = ans.at<cv::Vec3b>(i, j);
			a[0] = a[1] = a[2] = vv;
//          a[0] = int(v[0] / n) * n;
//          a[1] = int(v[1] / n) * n;
//          a[2] = int(v[2] / n) * n;
		}
	}
//  cv::medianBlur(ans, ans, 3);
//  cv::medianBlur(ans, ans, 5);
//  cv::medianBlur(ans, ans, 7);
	return ans;
}

ColorConstraint_sptr GetColorConstraint(cv::Mat& origin, cv::Mat& tmp)
{
	ColorConstraint_sptr res = ColorConstraint_sptr(new ColorConstraint);
	for (int i = 1; i < origin.cols; i++)
	{
		for (int j = 1; j < origin.rows - 1; j++)
		{
			if (tmp.at<uchar>(j + 1, i + 1))
			{
				res->AddPoint(i, j, origin.at<cv::Vec3b>(j, i));
			}
		}
	}
	return res;
}

