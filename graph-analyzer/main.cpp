#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include "headers/connected_components.h"
#include "headers/degrees_distribution_functions.h"
#include "headers/shortest_paths_functions.h"
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
	helper vect *transposed = graph->transposed;
	for (int i = 0; i < node_count; ++i)
	{
		int curr_node, neigbour;
		fscanf(input, "%d%d", &curr_node, &edge_count);
		for (int j = 0; j < edge_count; ++j)
		{
			fscanf(input, "%d", &neigbour);
			add_edge(graph, curr_node, neigbour);
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
	delete_cc_info(scc_info);


	// *** 3.3. Degree distriubution *** 
	printf("\n\n3.3.\n");

	printf("Degrees:\n");
	int group_sizes[] = { 1, 4, 10, 25 };
	for (int j = 0; j < 4; ++j)
	{
		const int group_size = group_sizes[j];
		vect *in_degs = get_in_degrees(graph, group_size);
		vect *out_degs = get_out_degrees(graph, group_size);
		for (int i = 0; i < in_degs->size(); ++i)
		{
			if ((*in_degs)[i] == 0 && (*out_degs)[i] == 0) continue;
			printf("[range %d-%d] IN: %d, OUT: %d\n", i * group_size, (i + 1) * group_size - 1, (*in_degs)[i], (*out_degs)[i]);
		}

		plot_degrees_hist(in_degs, out_degs, group_size);

		delete out_degs;
		delete in_degs;
	}

	// *** 3.4. Shortest paths *** 
	printf("\n\n3.4.\n");

	graph_metrics_t *metrics = find_metrics(graph);
	printf("Global avergage distance: %.3lf, Diameter: %d\n", metrics->global_avg, metrics->diameter);
	printf("Local metrics:\n");
	helper std::vector<double> *avg = metrics->avg;
	helper vect *eccentricities = metrics->eccentricities;
	for (int i = 0; i < graph->node_count; ++i)
	{
		printf("%d: Average distance = %.3lf, Eccentricity = %d\n", i, (*avg)[i], (*eccentricities)[i]);
	}

	const double d_sizes[4] = { 1, 0.5, 0.2, 0.1 };
	for (int i = 0; i < 4; ++i)
	{
		const double group_size = d_sizes[i];
		plot_metrics(metrics, group_size);
	}
	delete_metrics(metrics);

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