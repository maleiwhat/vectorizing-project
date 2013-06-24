#define _USE_MATH_DEFINES
#include <cmath>

#include "IFExtenstion.h"
#include "math\Quaternion.h"


double_vector ConvertToAngle(const double_vector& data)
{
	double_vector tmp = (data);
	double_vector ans;
	const int size = data.size();
	for (int i = 0; i < size; ++i)
	{
		int ileft = (i + 2) % size;
		int iright = (i - 2 + size) % size;
		double dy = tmp[iright] - tmp[ileft];
		double angle = atan2f(5, dy) / M_PI * 180 + 200;
		ans.push_back(angle);
	}
	return Smoothing(ans);
}

double_vector Smoothing(const double_vector& data)
{
	double_vector cps = data;
	double_vector newcps;
	for (int count = 0; count < 5; ++count)
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
	printf("lookline: %d\n", lookline);
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

double_vector ConvertToTest(const double_vector& data, double zero)
{
	const int size = data.size();
	double_vector tmp, tmp2;
	int bigger_zero = -1;
	int smaller_zero = -1;
	for (int i = 0; i < size; ++i)
	{
		if (data[i] > zero)
		{
			for (int j = i; j < size; ++j)
			{
				if (abs(data[j] - zero) < 1)
				{
					bigger_zero = i;
				}
			}
			break;
		}
	}
	if (bigger_zero == -1)
	{
		return data;
	}
	tmp2.insert(tmp2.end(), data.begin() + bigger_zero, data.end());
	tmp2.insert(tmp2.end(), data.begin(), data.begin() + bigger_zero);
	if (tmp2.back() > zero)
	{
		for (int i = size - 1; i > 0; --i)
		{
			if (abs(tmp2[i] - zero) < 1)
			{
				bigger_zero = i;
				break;
			}
		}
		tmp.insert(tmp.end(), tmp2.begin() + bigger_zero, tmp2.end());
		tmp.insert(tmp.end(), tmp2.begin(), tmp2.begin() + bigger_zero);
	}
	else
	{
		tmp = tmp2;
	}
	return tmp;
}
