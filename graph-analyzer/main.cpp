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

	delete in;
	delete out;
	delete[] scc_members;

	// degrees distribution
	printf("\n\n3.3\n");
	graph_t *transposed = create_transposed(graph);

	std::vector<int> *out_dist = get_degrees(graph);
	printf("OUT degrees:\n");
	for (int i = 0; i < node_count; ++i)
	{
		if (out_dist[i].size() > 0) printf("%d nodes have OUT degree %d\n", out_dist[i].size(), i);
	}
	std::vector<int> *in_dist = get_degrees(transposed);
	printf("IN degrees:\n");
	for (int i = 0; i < node_count; ++i)
	{
		if (in_dist[i].size() > 0) printf("%d nodes have IN degree %d\n", in_dist[i].size(), i);
	}

	// TODO: wyznaczenie współczynników funkcji potęgowej

	//delete[] in_dist;
	//delete[] out_dist;
	//delete_graph(transposed);
	//// shortest paths
	//printf("\n\n3.4\n");
	//double avg_dist_global = 0;
	//int diam = 0;
	//int *eccentricity = NULL;
	//double *avg_dist = NULL;
	//int **results = shortest_paths(graph, &avg_dist_global, &diam, &eccentricity, &avg_dist);
	//printf("Average distance: %.4lf\n", avg_dist_global);
	//printf("Diameter: %d\n", diam);
	//// TODO: wyznaczenie histogramów ekscentryczności i średnich odległości

	//for (int i = 0; i < node_count; ++i) free(results[i]);
	//free(results);
	//free(avg_dist);
	//free(eccentricity);

	// clustering factors
	printf("\n\n3.5\n");
	double global_clustering_factor = 0;
	double *clustering_factors = get_clustering_factors(graph, &global_clustering_factor);
	printf("Global clustering factor: %.4lf\n", global_clustering_factor);
	printf("Local clustering factors:\n");
	for (int i = 0; i < node_count; ++i)
	{
		printf("%d: %.4lf clustered\n", i, clustering_factors[i]);
	}

	free(clustering_factors);

	/* cleanup */
	delete_graph(graph);
	return 0;
}