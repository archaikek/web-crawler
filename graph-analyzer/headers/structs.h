#pragma once
#include "defines.h"
#include <vector>

typedef struct
{
	int node_count;
	std::vector<int> *edges;
} graph_t;

graph_t *create_graph(const int node_count);
void delete_graph(graph_t *graph);

inline void add_edge(graph_t *graph, const int src, const int dst)
{
	graph->edges[src].emplace_back(dst);
}