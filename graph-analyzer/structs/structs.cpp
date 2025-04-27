#include "../headers/structs.h"

graph_t *create_graph(const int node_count)
{
	graph_t *graph = (graph_t *)malloc(sizeof(graph_t));

	graph->node_count = node_count;
	helper std::vector<int> *edges = graph->edges = new std::vector<int>[node_count];

	return graph;
}
void delete_graph(graph_t *graph)
{
	delete[] graph->edges;
	free(graph);
}