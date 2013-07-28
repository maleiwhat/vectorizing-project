#define _USE_MATH_DEFINES
#include <cmath>
#include "IFExtenstion.h"
#include "math\Quaternion.h"

double_vector Curvature(const double_vector& angles)
{
	double_vector ans = angles;
	ans[0] = 0;

	for (int i = 1; i < ans.size(); i++)
	{
		ans[i] = angles[i] - angles[i - 1];
	}

	return ans;
}

double_vector Accumulation(const double_vector& line)
{
	double_vector ans = line;

	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = ans[i] + ans[i - 1];
	}

	return ans;
}

double_vector AbsAccumulation(const double_vector& line)
{
	double_vector ans = line;
	ans[0] = abs(ans[0]);

	for (int i = 1; i < line.size(); i++)
	{
		ans[i] = abs(ans[i]) + ans[i - 1];
	}

	return ans;
}

double_vector ConvertToAngle(const double_vector& data, double zero)
{
	zero -= 90;
	double_vector tmp = data;
	double_vector ans;
	{
		double dy = tmp[2] - tmp[0];
		double angle = atan2f(3, dy) / M_PI * 180 + zero;
		ans.push_back(angle);
		dy = tmp[3] - tmp[1];
		angle = atan2f(3, dy) / M_PI * 180 + zero;
		ans.push_back(angle);
	}
	for (int i = 2; i < data.size() - 2; ++i)
	{
		double dy = tmp[i + 2] - tmp[i - 2];
		double angle = atan2f(3, dy) / M_PI * 180 + zero;
		ans.push_back(angle);
	}
	{
		int last = data.size() - 2;
		double dy = tmp[last-1] - tmp[last - 3];
		double angle = atan2f(3, dy) / M_PI * 180 + zero;
		ans.push_back(angle);
		dy = tmp[last] - tmp[last - 1];
		angle = atan2f(3, dy) / M_PI * 180 + zero;
		ans.push_back(angle);
	}
	return SmoothingLen5(ans, 0, 2);
}

double_vector SmoothingLoop(const double_vector& data, double centroidRadio,
							int repeat)
{
	double_vector cps = data;
	double_vector newcps;
	for (int count = 0; count < repeat; ++count)
	{
		newcps.clear();
		int last = cps.size() - 1;
		if (cps.size() < 5)
		{
			continue;
		}
		newcps.push_back((cps[last] + cps[0] + cps[1]) / 3.0f);
		newcps.push_back((cps[0] + cps[1] + cps[2]) / 3.0f);
		for (int j = 2; j < cps.size() - 2; j ++)
		{
			auto vec = (cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
					   5.0f;
			newcps.push_back(vec);
		}
		newcps.push_back((cps[last - 2] + cps[last - 1] + cps[last]) / 3.0f);
		newcps.push_back((cps[last - 1] + cps[last] + cps[0]) / 3.0f);
		cps = newcps;
	}
	return newcps;
}

double_vector SmoothingLen3(const double_vector& data, double centroidRadio,
							int repeat)
{
	double_vector cps = data;
	double_vector newcps;
	double_vector centroids;
	for (int count = 0; count < repeat; ++count)
	{
		centroids.clear();
		newcps.clear();
		int last = cps.size() - 1;
		if (cps.size() < 5)
		{
			continue;
		}
		newcps.push_back((cps[0] * 2 + cps[1]) / 3.0f);
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j - 1] + cps[j + 1]) * 0.25;
			newcps.push_back(vec);
			centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1]) / 3.0f);
		}
		newcps.push_back((cps[last - 1] + cps[last] * 2) / 3.0f);
		cps = newcps;
		// move centroid
		newcps.clear();
		newcps.push_back(cps.front());
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			double nowCentroid = (cps[j] + cps[j + 1] + cps[j - 1]) / 3.0f;
			nowCentroid = centroids[j - 1] - nowCentroid;
			newcps.push_back(cps[j] + nowCentroid * centroidRadio);
		}
		newcps.push_back(cps.back());
		cps = newcps;
	}
	return newcps;
}

double_vector SmoothingLen5(const double_vector& data, double centroidRadio,
							int repeat)
{
	double_vector cps = data;
	double_vector newcps;
	double_vector centroids;
	for (int count = 0; count < repeat; ++count)
	{
		centroids.clear();
		newcps.clear();
		int last = cps.size() - 1;
		if (cps.size() < 5)
		{
			continue;
		}
		newcps.push_back((cps[0] * 2 + cps[1]) / 3.0f);
		newcps.push_back((cps[1] * 2 + cps[0] + cps[2]) * 0.25f);
		centroids.push_back((cps[0] + cps[1]  + cps[2]) / 3.0f);
		for (int j = 2; j < cps.size() - 2; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
					   6.0f;
			newcps.push_back(vec);
			centroids.push_back((cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
								 cps[j - 2]) / 5.0f);
		}
		centroids.push_back((cps[last] + cps[last - 1]  + cps[last - 2]) / 3.0f);
		newcps.push_back((cps[last - 1] * 2 + cps[last] + cps[last - 2]) * 0.25f);
		newcps.push_back((cps[last - 1] + cps[last] * 2) / 3.0f);
		cps = newcps;
		// move centroid
		newcps.clear();
		newcps.push_back(cps.front());
		double cert = (cps[0] + cps[1]  + cps[2]) / 3.0f;
		cert = centroids[0] - cert;
		newcps.push_back(cps[1] + cert);
		for (int j = 2; j < cps.size() - 2; j ++)
		{
			double nowCentroid = (cps[j] + cps[j + 1] + cps[j - 1] + cps[j + 2] +
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
	return newcps;
}

double_vector ConvertToSquareWave(const double_vector& data, int error,
								  double value, double zero)
{
	double_vector ans;
	const int size = data.size();
	for (int i = 0; i < size; ++i)
	{
		if (data[i] - zero > error)
		{
			ans.push_back(zero + value);
		}
		else if (zero - data[i] > error)
		{
			ans.push_back(zero - value);
		}
		else
		{
			ans.push_back(zero);
		}
	}
	return ans;
}

bool IsBlackLine(const double_vector& data, double zero)
{
	bool end = false;
	int lookline = 0;
	const int check_length = 15;
	const int size = data.size();
	for (int i = 0; i < size && !end; ++i)
	{
		if (data[i] > zero)
		{
			if (i == 0 && data.back() < zero)
			{
				lookline--;
			}
			// find zero point
			for (int j = i; j < size * 2; ++j)
			{
				if (abs(data[j % size] - zero) < 1)
				{
					i = j % size;
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
			for (int j = i; j < i + check_length; ++j)
			{
				if (data[j % size] < zero)
				{
					lookline++;
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
		}
	}
	if (lookline == 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsBrightLine(const double_vector& data, double zero /*= 290*/)
{
	bool end = false;
	int lookline = 0;
	const int check_length = 15;
	const int size = data.size();
	for (int i = 0; i < size && !end; ++i)
	{
		if (data[i] < zero)
		{
			if (i == 0)
			{
				for (int j = size - check_length; j < size; ++j)
				{
					if (data[j] < zero)
					{
						lookline--;
						break;
					}
				}
			}
			// find zero point
			for (int j = i; j < size * 2; ++j)
			{
				if (abs(data[j % size] - zero) < 1)
				{
					i = j % size;
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
			for (int j = i; j < i + check_length; ++j)
			{
				if (data[j % size] > zero)
				{
					lookline++;
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
		}
	}
	if (lookline == 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsShading(const double_vector& data, double zero)
{
	const int size = data.size();
	int jump = 0;
	int now = 0;
	for (int i = 0; i < size; ++i)
	{
		if (data[i] > zero)
		{
			if (now == 0)
			{
				now = 1;
			}
			else if (now == -1)
			{
				now = 1;
				jump++;
			}
		}
		else if (data[i] < zero)
		{
			if (now == 0)
			{
				now = -1;
			}
			else if (now == 1)
			{
				now = -1;
				jump++;
			}
		}
	}
	if (jump == 1 || jump == 2)
	{
		return true;
	}
	return false;
}


double_vector GetBlackLine(const double_vector& data, double zero /*= 290*/)
{
	double_vector ans;
	bool end = false;
	const int check_length = 15;
	const int size = data.size();
	for (int i = 0; i < size && !end; ++i)
	{
		if (data[i] > zero)
		{
			// find zero point
			for (int j = i; j < size * 2; ++j)
			{
				if (abs(data[j % size] - zero) < 1)
				{
					i = j % size;
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
			for (int j = i; j < i + check_length; ++j)
			{
				if (data[j % size] < zero)
				{
					ans.push_back(((j % size) * 360.0 / size + 180) / 180 * M_PI);
					if (j >= size)
					{
						end = true;
					}
					break;
				}
			}
		}
	}
	return ans;
}

double GetVariance(const double_vector& data)
{
	double sum = 0;
	for (int i = 0; i < data.size(); ++i)
	{
		sum += data[i];
	}
	sum /= data.size();
	double variance = 0;
	for (int i = 0; i < data.size(); ++i)
	{
		double v = sum - data[i];
		variance += v * v;
	}
	return variance;
}

double_vector LogSmooth(const double_vector& data, double zero)
{
	double_vector ans;
	const int size = data.size();
	for (int i = 0; i < size; ++i)
	{
		if (data[i] - zero >= 0)
		{
			ans.push_back(zero + 0.1 * exp(0.4 * (data[i] - zero)));
		}
		else
		{
			ans.push_back(zero - 0.1 * exp(0.4 * (zero - data[i])));
		}
	}
	return ans;
}

double_vector GetLineWidth(const double_vector& data, double lineWidth,
						   double zero /*= 290*/)
{
	double_vector ans;
	bool end = false;
	const int check_length = 179;
	const int size = data.size();
	for (int i = size / 2; i > size / 2 - check_length && !end; --i)
	{
		if (data[i] > zero)
		{
			for (int j = i; j >= 0; --j)
			{
				if (data[j] <= zero)
				{
					ans.push_back(lineWidth * 0.5 - j * lineWidth / 360.0);
					end = true;
					break;
				}
				else if (j == 0)
				{
					ans.push_back(lineWidth * 0.5 - j * lineWidth / 360.0);
					end = true;
				}
			}
		}
	}
	for (int i = size / 2; i < size / 2 + check_length; ++i)
	{
		if (data[i] < zero)
		{
			for (int j = i; j < size; ++j)
			{
				if (data[j] >= zero)
				{
					ans.push_back(j * lineWidth / 360.0 - lineWidth * 0.5);
					return ans;
				}
				else if (j == size - 1)
				{
					ans.push_back(j * lineWidth / 360.0 - lineWidth * 0.5);
					return ans;
				}
			}
		}
	}
	return double_vector();
}
