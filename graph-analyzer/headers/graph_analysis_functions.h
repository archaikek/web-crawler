#pragma once
#include "structs.h"

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

