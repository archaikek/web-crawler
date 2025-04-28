#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include <cstdio>

int main()
{
	/* Read the graph from the input file */
	FILE *input = fopen(input_path, "r");
	if (input == NULL)
	{
		printf("Couldn't open input file!\n");
		return -1;
	}
	FILE *results_file = fopen(output_path, "w");
	if (input == NULL)
	{
		printf("Couldn't open output file!\n");
		return -1;
	}
	pass_results_file(results_file);

	
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
	// node and edge counts
	fprintf(results_file, "\n\n3.1\n");
	fprintf(results_file, "Node count: %d\n", get_node_count(graph));
	fprintf(results_file, "Edge count: %d\n", get_edge_count(graph));

	// connected components analysis
	run_3_2(graph);

	// degrees distribution
	run_3_3(graph);

	// shortest paths
	run_3_4(graph);

	// clustering factors
	run_3_5(graph);

	// resistance tests
	run_3_6(graph);

	/* cleanup */
	delete_graph(graph);
	fclose(results_file);
	return 0;
}