#define _USE_MATH_DEFINES
#include <cmath>

#include "IFExtenstion.h"
#include "math\Quaternion.h"


double_vector ConvertToAngle(const double_vector& data)
{
	double_vector tmp = (data);
	double_vector ans;
	{
		double dy = tmp[1] - tmp[0];
		double angle = atan2f(2, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
		dy = tmp[2] - tmp[0];
		angle = atan2f(3, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
	}
	for (int i = 2; i < data.size() - 2; ++i)
	{
		double dy = tmp[i + 2] - tmp[i - 2];
		double angle = atan2f(5, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
	}
	{
		int last = data.size() - 1;
		double dy = tmp[last] - tmp[last - 2];
		double angle = atan2f(3, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
		dy = tmp[last] - tmp[last - 1];
		angle = atan2f(2, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
	}
	return SmoothingLen5(ans);
}

double_vector SmoothingLoop(const double_vector& data, int repeat)
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

double_vector SmoothingLen3(const double_vector& data, int repeat)
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
		newcps.push_back((cps[0] * 2 + cps[1]) / 3.0f);
		for (int j = 1; j < cps.size() - 1; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j - 1] + cps[j + 1]) * 0.25;
			newcps.push_back(vec);
		}
		newcps.push_back((cps[last - 1] + cps[last] * 2) / 3.0f);
		cps = newcps;
	}
	return newcps;
}

double_vector SmoothingLen5(const double_vector& data, int repeat)
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
		newcps.push_back((cps[0] * 2 + cps[1]) / 3.0f);
		newcps.push_back((cps[1] * 2 + cps[0] + cps[2]) * 0.25f);
		for (int j = 2; j < cps.size() - 2; j ++)
		{
			auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
					   6.0f;
			newcps.push_back(vec);
		}
		newcps.push_back((cps[last - 1] * 2 + cps[last] + cps[last - 2]) * 0.25f);
		newcps.push_back((cps[last - 1] + cps[last] * 2) / 3.0f);
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
			ans.push_back(zero + 0.1*exp(0.4*(data[i] - zero)));
		}
		else
		{
			ans.push_back(zero - 0.1*exp(0.4*(zero - data[i])));
		}
	}
	return ans;
}

double_vector GetLineWidth( const double_vector& data, double zero /*= 290*/ )
{
	double_vector ans;
	bool end = false;
	const int check_length = 100;
	const int size = data.size();
	for (int i = 0; i < size; ++i)
	{
		if (data[i] > zero)
		{
			for (int j = i; j < size && j < i + check_length; ++j)
			{
				if (data[j] < zero)
				{
					ans.push_back(i*10/360.0-5);
					i = j;
					end = true;
					break;
				}
			}
			if (end)
			{
				for (int j = i; j < size; ++j)
				{
					if (data[j] >= zero)
					{
						ans.push_back(j*10/360.0-5);
						return ans;
					}
				}
			}
		}
	}
	return ans;
}
