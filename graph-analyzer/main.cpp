#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include "headers/connected_components.h"
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

	int node_count, edge_count;
	fscanf(input, "%d", &node_count);
	graph_t *graph = create_graph(node_count);
	helper vect *edges = graph->edges;
	for (int i = 0; i < node_count; ++i)
	{
		int curr_node, neigbour;
		fscanf(input, "%d%d", &curr_node, &edge_count);
		for (int j = 0; j < edge_count; ++j)
		{
			fscanf(input, "%d", &neigbour);
			edges[curr_node].emplace_back(neigbour);
		}
	}

	// *** 3.1. Node and edge counts ***
	printf("3.1.\n");

	printf("Node count: %d\n", get_node_count(graph));
	printf("Edge count: %d\n", get_edge_count(graph));

	// *** 3.2. Weakly- and strongly-connected components in graph *** 
	printf("\n\n3.2.\n");

	cc_info_t *wcc_info = find_wcc(graph);
	printf("WCC count: %d\n", wcc_info->cc_contents->size());
	printf("WCCs: \n");
	for (auto wcc : *(wcc_info->cc_contents))
	{
		printf("%d (size = %d) <- ", wcc.first, wcc.second.size());
		for (auto node : wcc.second) printf("%d ", node);
		printf("\n");
	}
	delete_cc_info(wcc_info);

	cc_info_t *scc_info = find_scc(graph);
	int single_node_scc_count = 0;
	printf("\nSCC count: %d\n", scc_info->cc_contents->size());
	for (auto scc : *(scc_info->cc_contents))
	{
		if (scc.second.size() > 1)
		{
			printf("%d (size = %d) <- ", scc.first, scc.second.size());
			for (auto node : scc.second) printf("%d ", node);
			printf("\n");
		}
	}
	printf("\nSingle node SCCs: ");
	for (auto scc : *(scc_info->cc_contents))
	{
		if (scc.second.size() == 1)
		{
			printf("%d ", scc.first);
			++single_node_scc_count;
		}
	}
	printf("(%d total)\n", single_node_scc_count);


	// *** 3.3. Degree distriubution *** 
	printf("\n\n3.3.\n");

	// *** 3.4. Shortest paths *** 
	printf("\n\n3.4.\n");

	// *** 3.5. Clustering *** 
	printf("\n\n3.5.\n");

	// *** 3.6. Malfunction and attack resistance *** 
	printf("\n\n3.6.\n");

	// *** 3.7. Connectivity *** 
	printf("\n\n3.7.\n");

	// *** 4. PageRank *** 
	printf("\n\n4.\n");

	delete_graph(graph);
	fclose(input);
	return 0;
}