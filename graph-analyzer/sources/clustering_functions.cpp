#include "../headers/clustering_functions.h"

static char filename[250];
void set_clustering_filename(const char *new_filename)
{
	strcpy(filename, new_filename);
}

static int **paths2;
static int *visited, run_count;
static void init_clustering_vars(const int node_count)
{
	paths2 = (int **)malloc(node_count * sizeof(int *));
	for (int i = 0; i < node_count; ++i) paths2[i] = (int *)calloc(node_count, sizeof(int));
	visited = (int *)calloc(node_count, sizeof(int));
}
static void clear_clustering_vars(const int node_count)
{
	free(visited);
	for (int i = 0; i < node_count; ++i) free(paths2[i]);
	free(paths2);
}

static cluster_info_t *create_cluster_info(const int node_count)
{
	cluster_info_t *info = (cluster_info_t *)malloc(sizeof(cluster_info_t));
	info->global = 0;
	info->local = (double *)calloc(node_count, sizeof(double));
	info->node_count = node_count;
	return info;
}
void delete_cluster_info(cluster_info_t *info)
{
	free(info->local);
	free(info);
}

static void find_paths(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	for (int i = 0; i < node_count; ++i)
	{
		helper const int size = edges[i].size(); if (size == 0) continue;
		helper const int *neighbours = &(edges[i][0]);
		for (int j = 0; j < size; ++j)
		{
			helper const int neighbour = neighbours[j];
			helper const int size2 = edges[neighbour].size(); if (size2 == 0) continue;
			helper const int *neighbours2 = &(edges[neighbour][0]);
			for (int k = 0; k < size2; ++k) ++paths2[i][neighbours2[k]];
		}
	}
}
cluster_info_t *find_clustering(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;

	cluster_info_t *info = create_cluster_info(node_count);
	init_clustering_vars(node_count);
	find_paths(graph);

	long long int global_num = 0, global_den = 0;
	helper double *local = info->local;
	for (int i = 0; i < node_count; ++i)
	{
		int numerator = 0, denominator = 0;
		helper const int size = edges[i].size();
		for (int j = 0; j < size; ++j) numerator += paths2[i][edges[i][j]]; // closed paths of length 2
		for (int j = 0; j < node_count; ++j) denominator += paths2[i][j]; // all paths of length 2
		global_num += numerator;
		global_den += denominator;
		local[i] = double(numerator) / double(denominator == 0 ? 1 : denominator);
	}
	info->global = double(global_num) / global_den;

	clear_clustering_vars(node_count);
	return info;
}

void plot_clustering(const cluster_info_t *info, const double group_size)
{
	std::vector<double> *local = new std::vector<double>(info->local, info->local + info->node_count);
	std::pair<std::vector<double>, std::vector<int>> axes = make_histogram(local, group_size);
	coefficient_t coeff = regress_linear(&(axes.first), &(axes.second));
	std::vector<double> *line = make_line(&(axes.first), coeff, calculate_linear_fun);

#ifndef _DEBUG
	char title[250];
	plt::figure_size(800, 600);
	plt::plot(axes.first, axes.second, "r-");
	plt::plot(axes.first, *line, "y--");
	plt::xlabel("Local Clustering Factor");
	plt::ylabel("Number of nodes with given local clustering factor");
	sprintf(title, "Distribution of local clustering factors in graph (grouped by %.3lf)\napprox. %.3lfx + %.3lf; MSE=%.3lf\n", group_size, coeff.a, coeff.b, coeff.error);
	plt::title(title);
	plt::save(filename);
	plt::close();
	printf("%s", title);
#endif
}