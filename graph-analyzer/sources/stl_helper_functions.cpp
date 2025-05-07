#include "../headers/stl_helper_functions.h"

std::vector<double> *make_line(const std::vector<double> *X, coefficient_t coeff,
	double (*calc_fun)(const coefficient_t, const double))
{
	helper const int size = X->size();
	helper const double *data = &((*X)[0]);
	std::vector<double> *line = new std::vector<double>(size);
	helper double *line_vals = &((*line)[0]);
	for (int i = 0; i < size; ++i)
	{
		line_vals[i] = calc_fun(coeff, data[i]);
	}

	return line;
}