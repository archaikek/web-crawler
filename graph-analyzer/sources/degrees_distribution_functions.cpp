#include "../headers/degrees_distribution_functions.h"

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
	std::pair<double, double> in_coeff = regress_power(&(in_axes.first), &(in_axes.second));
	std::pair<double, double> out_coeff = regress_power(&(out_axes.first), &(out_axes.second));
	std::vector<double> *in_line = make_line(&(in_axes.first), in_coeff, calculate_power_fun);
	std::vector<double> *out_line = make_line(&(out_axes.first), out_coeff, calculate_power_fun);

#ifndef _DEBUG
	char in_filename[250], out_filename[250];
	sprintf(in_filename, "%s%s%.0lf.png", figures_path, "fig3_3_hist-IN-", group_size);
	sprintf(out_filename, "%s%s%.0lf.png", figures_path, "fig3_3_hist-OUT-", group_size);

	char title[80];
	plt::figure_size(800, 600);
	plt::loglog(in_axes.first, in_axes.second, "r.");
	plt::loglog(in_axes.first, *in_line, "y-");
	plt::xlabel("Node in-degree");
	plt::ylabel("Number of nodes with given in-degrees");
	sprintf(title, "Distribution of in-degrees in graph (grouped by %.0lf)", group_size);
	plt::title(title);
	plt::save(in_filename);
	plt::close();
	
	plt::figure_size(800, 600);
	plt::loglog(out_axes.first, out_axes.second, "g.");
	plt::loglog(out_axes.first, *out_line, "b-");
	plt::xlabel("Node out-degree");
	plt::ylabel("Number of nodes with given out-degrees");
	sprintf(title, "Distribution of out-degrees in graph (grouped by %.0lf)", group_size);
	plt::title(title);
	plt::save(out_filename);
	plt::close();
#endif // !_DEBUG

	delete out_line;
	delete in_line;
	delete out_degs;
	delete in_degs;
}
void plot_degrees_hist(const vect *in_degs, const vect *out_degs, const int group_size)
{
	char in_filename[250] = "", out_filename[250] = "";
	sprintf(in_filename, "%s%s%d.png", figures_path, "fig3_3_hist-IN-", group_size);
	sprintf(out_filename, "%s%s%d.png", figures_path, "fig3_3_hist-OUT-", group_size);
	printf("%s\n", in_filename);

	vect *x = new vect();
	for (int i = 0; i < in_degs->size(); ++i) x->emplace_back(i * group_size);
	std::pair<double, double> in_line = regress_power(x, in_degs), out_line = regress_power(x, out_degs);
	printf("Linear approximation (groups of %d): IN = %.3lfx + %.3lf, OUT = %.3lfx + %.3lf\n", group_size, in_line.first, in_line.second, out_line.first, out_line.second);

	std::vector<double> *y_in = new std::vector<double>();
	std::vector<double> *y_out = new std::vector<double>();
	double in_error = 0, out_error = 0;
	for (int i = 0; i < x->size(); ++i)
	{
		double in = calculate_power_fun(in_line, (*x)[i]);
		double out = calculate_power_fun(out_line, (*x)[i]);

		if (i > 0)
		{
			in_error += sq(in - (*in_degs)[i]);
			out_error += sq(out - (*out_degs)[i]);
		}
		y_in->emplace_back(in);
		y_out->emplace_back(out);
	}
	printf("Approximation errors: IN_ERR = %.3lf, OUT_ERR = %.3lf\n\n", in_error / x->size(), out_error / x->size());
#ifndef _DEBUG
	char title[80];
	plt::figure_size(800, 600);
	plt::loglog(*x, *y_in, "y-");
	plt::loglog(*x, *in_degs, "r.");
	plt::xlabel("Node degree");
	plt::ylabel("Number of nodes with given degree");
	sprintf(title, "Distribution of in-degrees in graph (grouped by %d)", group_size);
	plt::title(title);
	plt::save(in_filename);
	plt::close();

	plt::figure_size(800, 600);
	plt::loglog(*x, *y_out, "g-");
	plt::loglog(*x, *in_degs, "b.");
	plt::xlabel("Node degree");
	plt::ylabel("Number of nodes with given degree");
	sprintf(title, "Distribution of out-degrees in graph (grouped by %d)", group_size);
	plt::title(title);
	plt::save(out_filename);
	plt::close();
#endif // !_DEBUG
	delete y_out;
	delete y_in;
	delete x;
}