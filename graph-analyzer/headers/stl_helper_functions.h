#pragma once

#include "defines.h"

typedef struct
{
	double a, b;
	double error;
} coefficient_t;

template <typename T>
void remove_duplicates(std::vector<T> *v)
{
	std::unordered_set<T> *temp = new std::unordered_set<T>(v->begin(), v->end());
	v->clear();
	v->insert(v->begin(), temp->begin(), temp->end());
	delete temp;
}

inline double calculate_power_fun(const coefficient_t coeff, const double x)
{
	return pow(x, coeff.a) * pow(10, coeff.b);
}
inline double calculate_linear_fun(const coefficient_t coeff, const double x)
{
	return x * coeff.a + coeff.b;
}
std::vector<double> *make_line(const std::vector<double> *X, coefficient_t coeff,
	double (*calc_fun)(const coefficient_t, const double));
template <typename data_type_x, typename data_type_y>
coefficient_t regress_power(const std::vector<data_type_x> *X, const std::vector<data_type_y> *Y)
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
	double a = (s * sxy - sx * sy) / delta, b = (sxx * sy - sx * sxy) / delta;
	double error = 0;
	for (int i = 0; i < data_size; ++i)
	{
		const double x = double((*X)[i]);
		const double y = double((*Y)[i]);
		if (x < 0.9 || y < 0.9) continue;
		double logx = log10(x);
		double logy = log10(y);
		error += sq(logy - a * logx - b);
	}
	error /= s;

	return {a, b, error};
}
template <typename data_type_x, typename data_type_y>
coefficient_t regress_linear(const std::vector<data_type_x> *X, const std::vector<data_type_y> *Y)
{
	helper const int data_size = X->size();
	double s = data_size;
	double sx = 0, sy = 0, sxx = 0, sxy = 0, syy = 0;

	for (int i = 0; i < data_size; ++i)
	{
		const double x = double((*X)[i]);
		const double y = double((*Y)[i]);
		sx += x;
		sy += y;
		sxx += sq(x);
		sxy += x * y;
		syy += sq(y);
	}
	double delta = s * sxx - sq(sx);
	double a = (s * sxy - sx * sy) / delta, b = (sxx * sy - sx * sxy) / delta;
	double error = 0;
	for (int i = 0; i < data_size; ++i)
	{
		const double x = double((*X)[i]);
		const double y = double((*Y)[i]);
		error += sq(y - a * x - b);
	}
	error /= data_size;

	return {a, b, error};
}
template <typename data_type>
std::pair<std::vector<double>, std::vector<int>> make_histogram(const std::vector<data_type> *Y, const double group_size)
{
	std::pair<std::vector<double>, std::vector<int>> result;

	double max_val = -1e9, min_val = 1e9;
	helper const int data_size = Y->size();
	helper const data_type *data = &((*Y)[0]);
	for (int i = 0; i < data_size; ++i)
	{
		max_val = std::max(max_val, double(data[i]));
		min_val = std::min(min_val, double(data[i]));
	}

	const int result_size = int(ceil((max_val - min_val) / group_size)) + 2;
	result.first.resize(result_size);
	result.second.resize(result_size);
	for (int i = 0; i < result_size; ++i)
	{
		result.second[i] = 0;
		result.first[i] = min_val + i * group_size;
	}
	for (int i = 0; i < data_size; ++i)
	{
		++result.second[int(floor(double(data[i] - min_val) / group_size))];
	}

	return result;
}