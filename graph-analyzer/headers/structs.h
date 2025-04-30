#pragma once
#include "defines.h"

typedef std::vector<int> vect;
typedef struct
{
	int node_count;
	vect *edges, *transposed;
} graph_t;

graph_t *create_graph(const int node_count);
void delete_graph(graph_t *graph);

std::pair<vect, vect> *get_node_copy(const graph_t *graph, const int node);
void remove_node(graph_t *graph, const int node);

inline void add_edge(graph_t *graph, const int src, const int dst)
{
	graph->edges[src].emplace_back(dst);
	graph->transposed[dst].emplace_back(src);
}

inline int get_node_count(const graph_t *graph)
{
	return graph->node_count;
}
int get_edge_count(const graph_t *graph);

//void print_graph(const graph_t *graph);