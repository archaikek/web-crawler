#pragma once
#include "stl_helper_functions.h"
#include <cmath>
#include <queue>
#include <algorithm>

inline int get_node_count(const graph_t *graph)
{
	return graph->node_count;
}
inline int get_edge_count(const graph_t *graph)
{
	int result = 0;

	helper int node_count = graph->node_count;
	helper std::vector<int> *edges = graph->edges;
	for (int i = 0; i < node_count; ++i)
	{
		result += edges[i].size();
	}

	return result;
}

graph_t *create_scc(const graph_t *graph, std::vector<int> **out__scc_members);
std::vector<int> *get_out_components(const graph_t *graph);
std::vector<int> *get_in_components(const graph_t *graph);

std::vector<int> *get_degrees(const graph_t *graph);


int **shortest_paths(const graph_t *graph, double *out__avg_dist_global, int *out__diam, int **out__eccentricity, double **out__avg_dist);

int **create_squared(const graph_t *graph);
double *get_clustering_factors(const graph_t *graph, double *out__global_cluster);