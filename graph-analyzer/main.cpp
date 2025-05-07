#define _CRT_SECURE_NO_WARNINGS
#include "headers/graph_analysis_functions.h"
#include "headers/connected_components.h"
#include "headers/degrees_distribution_functions.h"
#include "headers/shortest_paths_functions.h"
#include "headers/clustering_functions.h"
#include "headers/resistance_check_functions.h"
#include "headers/pagerank.h"
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
	//printf("3.1.\n");

	//printf("Node count: %d\n", get_node_count(graph));
	//printf("Edge count: %d\n", get_edge_count(graph));

	//// *** 3.2. Weakly- and strongly-connected components in graph *** 
	//printf("\n\n3.2.\n");

	//cc_info_t *wcc_info = find_wcc(graph);
	//int single_node_wcc_count = 0;
	//printf("\nWCC count: %d\n", wcc_info->cc_contents->size());
	//for (auto wcc : *(wcc_info->cc_contents))
	//{
	//	if (wcc.second.size() == 1) continue;
	//	printf("%d (size = %d) <- ", wcc.first, wcc.second.size());
	//	for (auto node : wcc.second) printf("%d ", node);
	//	printf("\n");
	//}
	//printf("\nSingle node WCCs: ");
	//for (auto scc : *(wcc_info->cc_contents))
	//{
	//	if (scc.second.size() > 1) continue;
	//	printf("%d (", scc.first);
	//	for (int i = 0; i < edges[scc.first].size(); ++i)
	//	{
	//		if (i > 0) printf(" ");
	//		printf("%d", edges[scc.first][i]);
	//	}
	//	printf(") ");
	//	++single_node_wcc_count;
	//}
	//printf("(%d total)\n", single_node_wcc_count);

	//cc_info_t *scc_info = find_scc(graph);
	//int single_node_scc_count = 0;
	//printf("\nSCC count: %d\n", scc_info->cc_contents->size());
	//for (auto scc : *(scc_info->cc_contents))
	//{
	//	if (scc.second.size() == 1) continue;
	//	printf("%d (size = %d) <- ", scc.first, scc.second.size());
	//	for (auto node : scc.second) printf("%d ", node);
	//	printf("\n");
	//}
	//printf("\nSingle node SCCs: ");
	//for (auto scc : *(scc_info->cc_contents))
	//{
	//	if (scc.second.size() > 1) continue;
	//	printf("%d (", scc.first);
	//	for (int i = 0; i < edges[scc.first].size(); ++i)
	//	{
	//		if (i > 0) printf(" ");
	//		printf("%d", edges[scc.first][i]);
	//	}
	//	printf(") ");
	//	++single_node_scc_count;
	//}
	//printf("(%d total)\n", single_node_scc_count);
	//vect *in_components = find_in_components(scc_info);
	//printf("IN components: ");
	//for (int component : (*in_components)) printf("%d ", component);
	//vect *out_components = find_out_components(scc_info);
	//printf("\nOUT components: ");
	//for (int component : (*out_components)) printf("%d ", component);
	//printf("\n");

	//delete out_components;
	//delete in_components;
	//delete_cc_info(scc_info);


	//// *** 3.3. Degree distriubution *** 
	//printf("\n\n3.3.\n");

	//printf("Degrees:\n");
	//int group_sizes[] = { 1, 4, 10, 25 };
	//for (int i = 0; i < 4; ++i)
	//{
	//	helper const double group_size = group_sizes[i];
	//	char in_filename[250], out_filename[250];
	//	sprintf(in_filename, "%s%s%.0lf.png", figures_path, "fig3_3_hist-IN-", group_size);
	//	sprintf(out_filename, "%s%s%.0lf.png", figures_path, "fig3_3_hist-OUT-", group_size);
	//	set_in_filename(in_filename);
	//	set_out_filename(out_filename);
	//	plot_degrees(graph, double(group_size));
	//}

	//// *** 3.4. Shortest paths *** 
	//printf("\n\n3.4.\n");

	//graph_metrics_t *metrics = find_metrics(graph);
	//printf("Global avergage distance: %.3lf, Diameter: %d\n", metrics->global_avg, metrics->diameter);
	//printf("Local metrics:\n");
	//helper std::vector<double> *avg = metrics->avg;
	//helper vect *eccentricities = metrics->eccentricities;
	//for (int i = 0; i < graph->node_count; ++i)
	//{
	//	printf("%d: Average distance = %.3lf, Eccentricity = %d\n", i, (*avg)[i], (*eccentricities)[i]);
	//}

	//const double d_sizes[6] = { 1, 0.5, 0.2, 0.1, 0.04, 0.01 };
	//for (int i = 0; i < 6; ++i)
	//{
	//	const double group_size = d_sizes[i];
	//	char filename[250] = "";
	//	sprintf(filename, "%s%s%.2lf.png", figures_path, "fig3_4_hist-AVG-", group_size);
	//	set_shortest_paths_filename(filename);
	//	plot_metrics(metrics, group_size);
	//}

	//char ecc_filename[250] = "";
	//sprintf(ecc_filename, "%s%s.png", figures_path, "fig3_4_hist-ECC");
	//set_shortest_paths_filename(ecc_filename);
	//plot_eccenticities(metrics);

	//delete_metrics(metrics);

	//// *** 3.5. Clustering *** 
	//printf("\n\n3.5.\n");

	//cluster_info_t *info = find_clustering(graph);
	//printf("Global clustering factor: %.3lf\n", info->global);
	//printf("Local clustering factors:\n");
	//for (int i = 0; i < node_count; ++i)
	//{
	//	printf("%d: %.3lf clustered\n", i, info->local[i]);
	//}
	//for (int i = 3; i < 6; ++i)
	//{
	//	const double group_size = d_sizes[i] / 5;
	//	char filename[250] = "";
	//	sprintf(filename, "%s%s%.3lf.png", figures_path, "fig3_5_hist-LOC-", group_size);
	//	set_clustering_filename(filename);
	//	plot_clustering(info, group_size);
	//}

	//delete_cluster_info(info);

	// *** 3.6. Malfunction and attack resistance *** 
	printf("\n\n3.6.\n");

	vect *failures = new vect({ 44, 111, 444, 1111, 2222, 3333});
	run_random_failures(graph, failures);
	run_attacks(graph, failures);
	delete failures;

	// *** 3.7. Connectivity *** 
	printf("\n\n3.7.\n");


	// *** 4. PageRank *** 
	printf("\n\n4.\n");
	char pr_filename[250] = "";
	sprintf(pr_filename, "%sfig4-pagerank", figures_path);
	double factors[8] = { 0.2, 0.5, 0.75, 0.85, 0.95, 1, 1.001, 1.1 };
	for (int j = 0; j < 8; ++j)
	{
		const double factor = factors[j];

		double *pagerank = get_pagerank(graph, factor, 1e-10);
		if (pagerank == NULL) continue;
		//for (int i = 0; i < node_count; ++i)
		//{
		//	printf("PageRank[%d] = %.9lf\n", i, pagerank[i]/* * node_count*/);
		//}
		sprintf(pr_filename, "%sfig4-pagerank-%.3lf.png", figures_path, factor);
		set_pagerank_filename(pr_filename);
		printf("??? %s\n", pr_filename);
		std::vector<double> *v_pagerank = new std::vector<double>(pagerank, pagerank + node_count);
		plot_pagerank(v_pagerank, 0.01);

		delete v_pagerank;
		free(pagerank);
	}

	delete_graph(graph);
	fclose(input);
	return 0;
}