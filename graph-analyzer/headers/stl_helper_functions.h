#pragma once

#include "defines.h"

template <typename T>
static void remove_duplicates(std::vector<T> *v);

inline double calculate_power_fun(const std::pair<double, double> coeff, const double x)
{
	return pow(x, coeff.first) * pow(10, coeff.second);
}
inline double calculate_linear_fun(const std::pair<double, double> coeff, const double x)
{
	return x * coeff.first + coeff.second;
}
std::vector<double> *make_line(const std::vector<double> *X, const std::pair<double, double> coeff, 
	double (*calc_fun)(const std::pair<double, double>, const double));
template <typename data_type_x, typename data_type_y>
std::pair<double, double> regress_power(const std::vector<data_type_x> *X, const std::vector<data_type_y> *Y)
{
	helper const int data_size = X->size();
	double s = data_size;
	double sx = 0, sy = 0, sxx = 0, sxy = 0, syy = 0;

	for (int i = 0; i < data_size; ++i)
	{
		const double x = double((*X)[i]);
		const double y = double((*Y)[i]);
		if (x < 0.9 || y < 0.9)
		{
			--s;
			continue;
		}
		double logx = log10(x);
		double logy = log10(y);


		sx += logx;
		sy += logy;
		sxx += sq(logx);
		sxy += logx * logy;
		syy += sq(logy);
	}

	double delta = s * sxx - sq(sx);
	return std::make_pair((s * sxy - sx * sy) / delta, (sxx * sy - sx * sxy) / delta);
}
template <typename data_type_x, typename data_type_y>
std::pair<double, double> regress_linear(const std::vector<data_type_x> *X, const std::vector<data_type_y> *Y)
{
	helper const int data_size = X->size();
	double s = data_size;
	double sx = 0, sy = 0, sxx = 0, sxy = 0, syy = 0;

	for (int i = 0; i < data_size; ++i)
	{
		const double x = double((*X)[i]);
		const double y = double((*Y)[i]);
		if (x < 0.9 || y < 0.9)
		{
			--s;
			continue;
		}

		sx += x;
		sy += y;
		sxx += sq(x);
		sxy += x * y;
		syy += sq(y);
	}

	double delta = s * sxx - sq(sx);
	return std::make_pair((s * sxy - sx * sy) / delta, (sxx * sy - sx * sxy) / delta);
}
template <typename data_type>
std::pair<std::vector<double>, std::vector<int>> make_histogram(const std::vector<data_type> *Y, const double group_size)
{
	std::pair<std::vector<double>, std::vector<int>> result;

	double max_val = 0;
	helper const int data_size = Y->size();
	helper const data_type *data = &((*Y)[0]);
	for (int i = 0; i < data_size; ++i)
	{
		max_val = std::max(max_val, double(data[i]));
	}

	const int result_size = int(ceil(max_val / group_size)) + 2;
	result.first.resize(result_size);
	result.second.resize(result_size);
	for (int i = 0; i < result_size; ++i)
	{
		result.second[i] = 0;
		result.first[i] = i * group_size;
	}
	for (int i = 0; i < data_size; ++i)
	{
		++result.second[int(floor(double(data[i]) / group_size))];
	}

	return result;
}