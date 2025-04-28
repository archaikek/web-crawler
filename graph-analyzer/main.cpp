#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include <cstdio>

int main()
{
	/* Read the graph from the input file */
	FILE *input = fopen(input_path, "r");
	if (input == NULL)
	{
		printf("Couldn't find input file!\n");
		return -1;
	}
	
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
	printf("\n\n3.1\n");
	printf("Node count: %d\n", get_node_count(graph));
	printf("Edge count: %d\n", get_edge_count(graph));

	// connected components analysis
	printf("\n\n3.2\n");
	std::vector<int> *scc_members = NULL;
	graph_t *scc = create_scc(graph, &scc_members);

	printf("WCC count: 1 (by definition)\n");
	printf("SCC count: %d\n", get_node_count(scc));
	printf("SCCs by nodes:\n");
	for (int i = 0; i < scc->node_count; ++i)
	{
		printf("%d <-", i);
		for (int j = 0; j < scc_members[i].size(); ++j)
		{
			printf(" %d", scc_members[i][j]);
		}
		printf("\n(%d total)\n", scc_members[i].size());
	}

	std::vector<int> *in = get_in_components(scc), *out = get_out_components(scc);
	printf("IN components: \n");
	for (int i = 0; i < in->size(); ++i)
	{
		printf("%d ", (*in)[i]);
	}
	printf("\n(%d total)\n", in->size());
	printf("OUT components: \n");
	for (int i = 0; i < out->size(); ++i)
	{
		printf("%d ", (*out)[i]);
	}
	printf("\n(%d total)\n", out->size());

	//print_graph(scc);

	// degrees distribution
	printf("\n\n3.3\n");
	graph_t *transposed = create_transposed(graph);



	/* cleanup */
	delete_graph(transposed);
	delete in;
	delete out;
	delete[] scc_members;
	delete_graph(graph);

	return 0;
}