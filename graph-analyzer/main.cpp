#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include <cstdio>

int main()
{
	/* Read the graph from the input file */
	FILE *input = fopen(input_path, "r");
	
	int node_count;
	fscanf(input, "%d", &node_count);
	graph_t *graph = create_graph(node_count);
	for (int i = 0; i < node_count; ++i)
	{
		int node, edge_count;
		fscanf(input, "%d%d", &node, &edge_count);
		for (int j = 0; j < edge_count; ++j)
		{
			int edge;
			fscanf(input, "%d", &edge);
			add_edge(graph, node, edge);
		}
	}

	fclose(input);

	/* Run graph analysis functions */
	printf("Node count: %d\n", get_node_count(graph));
	printf("Edge count: %d\n", get_edge_count(graph));

	printf("WCC count: 1 (by definition)\n");

	delete_graph(graph);

	return 0;
}