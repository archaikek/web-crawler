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

void print_graph(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	printf("Node count: %d\nEdges:\n", node_count);
	for (int i = 0; i < node_count; ++i)
	{
		helper const int size = edges[i].size();
		printf("%d ->", i);
		for (int j = 0; j < size; ++j)
		{
			printf(" %d", edges[i][j]);
		}
		printf("\n\n");
	}
}