#include "../headers/stl_helper_functions.h"

template <typename T> 
static void remove_duplicates(std::vector<T> *v)
{
	std::unordered_set<T> *temp = new std::unordered_set<T>(v->begin(), v->end());
	v->clear();
	v->insert(v->begin(), temp->begin(), temp->end());
	delete temp;
}

std::vector<double> *make_line(const std::vector<double> *X, const std::pair<double, double> coeff, 
	double (*calc_fun)(const std::pair<double, double>, const double))
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