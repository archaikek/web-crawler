#include "../headers/degrees_distribution_functions.h"

static char in_filename[250], out_filename[250];

void set_in_filename(const char *filename)
{
	strcpy(in_filename, filename);
}
void set_out_filename(const char *filename)
{
	strcpy(out_filename, filename);
}

vect *get_in_degrees(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *transposed = graph->transposed;
	vect *result = new vect(node_count);

	helper int *degs = &((*result)[0]);
	for (int i = 0; i < node_count; ++i) degs[i] = transposed[i].size();

	return result;
}
vect *get_out_degrees(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	vect *result = new vect(node_count);

	helper int *degs = &((*result)[0]);
	for (int i = 0; i < node_count; ++i) degs[i] = edges[i].size();

	return result;
}

void plot_degrees(const graph_t *graph, const double group_size)
{
	vect *in_degs = get_in_degrees(graph);
	vect *out_degs = get_out_degrees(graph);
	std::pair<std::vector<double>, std::vector<int>> in_axes = make_histogram(in_degs, group_size);
	std::pair<std::vector<double>, std::vector<int>> out_axes = make_histogram(out_degs, group_size);
	coefficient_t in_coeff = regress_power(&(in_axes.first), &(in_axes.second));
	coefficient_t out_coeff = regress_power(&(out_axes.first), &(out_axes.second));
	std::vector<double> *in_line = make_line(&(in_axes.first), in_coeff, calculate_power_fun);
	std::vector<double> *out_line = make_line(&(out_axes.first), out_coeff, calculate_power_fun);

#ifndef _DEBUG
	char title[250];
	plt::figure_size(800, 600);
	plt::loglog(in_axes.first, in_axes.second, "r.");
	plt::loglog(in_axes.first, *in_line, "y-");
	plt::xlabel("Node in-degree");
	plt::ylabel("Number of nodes with given in-degrees");
	sprintf(title, "Distribution of in-degrees in graph (grouped by %.0lf)\napprox. x^%.3lf * %.3lf; MSE=%.3lf\n", group_size, in_coeff.a, pow(10, in_coeff.b), in_coeff.error);
	plt::title(title);
	plt::save(in_filename);
	plt::close();
	printf("%s", title);

	plt::figure_size(800, 600);
	plt::loglog(out_axes.first, out_axes.second, "g.");
	plt::loglog(out_axes.first, *out_line, "b-");
	plt::xlabel("Node out-degree");
	plt::ylabel("Number of nodes with given out-degrees");
	sprintf(title, "Distribution of out-degrees in graph (grouped by %.0lf)\napprox. x^%.3lf * %.3lf; MSE=%.3lf\n", group_size, out_coeff.a, pow(10, out_coeff.b), out_coeff.error);
	plt::title(title);
	plt::save(out_filename);
	plt::close();
	printf("%s", title);

#endif // !_DEBUG

	delete out_line;
	delete in_line;
	delete out_degs;
	delete in_degs;
}