#include "../headers/pagerank.h"

static char filename[250];
void set_pagerank_filename(const char *new_filename)
{
	strcpy(filename, new_filename);
}

static inline int this_iter(const int iter)
{
	return iter & 1;
}
static inline int prev_iter(const int iter)
{
	return (iter & 1) ^ 1;
}
double *get_pagerank(const graph_t *graph, const double damping, const double precision)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;

	const double limit = node_count;
	double max_diff = precision + 1;
	double *results[2] = { (double *)malloc(node_count * sizeof(double)) , (double *)malloc(node_count * sizeof(double)) };
	for (int i = 0; i < node_count; ++i) results[0][i] = 1;
	int iter = 0;

	while (max_diff > precision)
	{
		++iter;
		if (iter > limit)
		{
			free(results[0]);
			free(results[1]);
			return NULL;
		}
		max_diff = 0;
		helper double *curr_result = results[this_iter(iter)];
		helper double *prev_result = results[prev_iter(iter)];
		memset(curr_result, 0, node_count * sizeof(double));
		for (int i = 0; i < node_count; ++i)
		{
			helper const int size = edges[i].size(); if (size == 0) continue;
			helper const int *neighbours = &(edges[i][0]);
			for (int j = 0; j < size; ++j) curr_result[neighbours[j]] += prev_result[i] / size;
		}
		for (int i = 0; i < node_count; ++i)
		{
			curr_result[i] = damping * curr_result[i] + (1 - damping);
			max_diff = std::max(max_diff, abs(curr_result[i] - prev_result[i]));
		}
		printf("??? iter = %d, max_diff = %lf\n", iter, max_diff);
	}

	free(results[prev_iter(iter)]);
	return results[this_iter(iter)];
}
void plot_pagerank(std::vector<double> *pagerank, const double group_size)
{
	std::pair<std::vector<double>, std::vector<int>> axes = make_histogram(pagerank, group_size);
	coefficient_t coeff = regress_power(&(axes.first), &(axes.second));
	std::vector<double> *line = make_line(&(axes.first), coeff, calculate_power_fun);

#ifndef _DEBUG
	char title[250];
	plt::figure_size(800, 600);
	plt::loglog(axes.first, axes.second, "r.");
	plt::loglog(axes.first, *line, "y--");
	plt::xlabel("Pagerank value");
	plt::ylabel("Number of nodes with given PageRank");
	sprintf(title, "PageRank distribution in graph (grouped by %.3lf)\napprox. %.3lfx + %.3lf; MSE=%.3lf", group_size, coeff.a, coeff.b, coeff.error);
	plt::title(title);
	plt::save(filename);
	plt::close();
	printf("%s", title);
#endif
}