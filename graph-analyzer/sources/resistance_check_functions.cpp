#include "../headers/resistance_check_functions.h"

static int *random_nodes, *biggest_nodes, *sizes;
static bool comp(int a, int b)
{
	return sizes[a] > sizes[b];
}
static void init_resistance_vars(const graph_t *graph)
{
	srand(2221);
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	helper const vect *transposed = graph->transposed;
	random_nodes = (int *)malloc(node_count * sizeof(int));
	biggest_nodes = (int *)malloc(node_count * sizeof(int));
	sizes = (int *)malloc(node_count * sizeof(int));
	
	for (int i = 0; i < node_count; ++i)
	{
		random_nodes[i] = biggest_nodes[i] = i;
		sizes[i] = edges[i].size() + transposed[i].size();
	}
	std::sort(biggest_nodes, biggest_nodes + node_count, comp);
	std::random_shuffle(random_nodes, random_nodes + node_count);
}
static void clear_resistance_vars()
{
	free(sizes);
	free(biggest_nodes);
	free(random_nodes);
}

void run_random_failures(const graph_t *original, vect *failure_sizes)
{
	FILE *out = fopen(output_path, "w");
	init_resistance_vars(original);

	vect *wcc_counts = new vect(), *scc_counts = new vect();
	vect *diameters = new vect();
	std::vector<double> *avgs = new std::vector<double>();
	vect *articulation_point_counts = new vect(), *articulation_pair_counts = new vect();
	for (int failure_size : (*failure_sizes))
	{
		vect *disabled_nodes = new vect(random_nodes, random_nodes + failure_size);
		graph_t *graph = create_reduced_graph(original, disabled_nodes);
		fprintf(out, "\n\nRUNNING WITH %d FAILED NODES: ", disabled_nodes->size());
		for (int failed_node : (*disabled_nodes)) fprintf(out, "%d ", failed_node);
		fprintf(out, "\n");

		helper int node_count = graph->node_count;
		helper vect *edges = graph->edges;
		helper vect *transposed = graph->transposed;
		// 3.2
		{
			cc_info_t *wcc_info = find_wcc(graph);
			int single_node_wcc_count = 0;
			fprintf(out, "\nWCC count: %d\n", wcc_info->cc_contents->size());
			wcc_counts->emplace_back(wcc_info->cc_contents->size());
			for (auto wcc : *(wcc_info->cc_contents))
			{
				if (wcc.second.size() == 1) continue;
				fprintf(out, "%d (size = %d) <- ", wcc.first, wcc.second.size());
				for (auto node : wcc.second) fprintf(out, "%d ", node);
				fprintf(out, "\n");
			}
			fprintf(out, "\nSingle node WCCs: ");
			for (auto scc : *(wcc_info->cc_contents))
			{
				if (scc.second.size() > 1) continue;
				fprintf(out, "%d (", scc.first);
				for (int i = 0; i < edges[scc.first].size(); ++i)
				{
					if (i > 0) fprintf(out, " ");
					fprintf(out, "%d", edges[scc.first][i]);
				}
				fprintf(out, ") ");
				++single_node_wcc_count;
			}
			fprintf(out, "(%d total)\n", single_node_wcc_count);

			cc_info_t *scc_info = find_scc(graph);
			int single_node_scc_count = 0;
			fprintf(out, "\nSCC count: %d\n", scc_info->cc_contents->size());
			scc_counts->emplace_back(scc_info->cc_contents->size());
			for (auto scc : *(scc_info->cc_contents))
			{
				if (scc.second.size() == 1) continue;
				fprintf(out, "%d (size = %d) <- ", scc.first, scc.second.size());
				for (auto node : scc.second) fprintf(out, "%d ", node);
				fprintf(out, "\n");
			}
			fprintf(out, "\nSingle node SCCs: ");
			for (auto scc : *(scc_info->cc_contents))
			{
				if (scc.second.size() > 1) continue;
				fprintf(out, "%d (", scc.first);
				for (int i = 0; i < edges[scc.first].size(); ++i)
				{
					if (i > 0) fprintf(out, " ");
					fprintf(out, "%d", edges[scc.first][i]);
				}
				fprintf(out, ") ");
				++single_node_scc_count;
			}
			fprintf(out, "(%d total)\n", single_node_scc_count);
			vect *in_components = find_in_components(scc_info);
			fprintf(out, "IN components: ");
			for (int component : (*in_components)) fprintf(out, "%d ", component);
			vect *out_components = find_out_components(scc_info);
			fprintf(out, "\nOUT components: ");
			for (int component : (*out_components)) fprintf(out, "%d ", component);
			fprintf(out, "\n");

			delete out_components;
			delete in_components;
			delete_cc_info(scc_info);
		}
		// 3.3
		{
			helper const double group_size = 4;
			char in_filename[250], out_filename[250];
			sprintf(in_filename, "%sfig3_6_rand_%d_hist-IN-%.0lf.png", figures_path, failure_size, group_size);
			sprintf(out_filename, "%sfig3_6_rand_%d_hist-OUT-%.0lf.png", figures_path, failure_size, group_size);
			set_in_filename(in_filename);
			set_out_filename(out_filename);
			plot_degrees(graph, group_size);
		}
		// 3.4
		{
			graph_metrics_t *metrics = find_metrics(graph);
			fprintf(out, "Global avergage distance: %.3lf, Diameter: %d\n", metrics->global_avg, metrics->diameter);
			diameters->emplace_back(metrics->diameter);
			avgs->emplace_back(metrics->global_avg);

			helper const double group_size = 0.04;
			char filename[250] = "";
			sprintf(filename, "%sfig3_6_rand_%d_hist-AVG-%.2lf.png", figures_path, failure_size, group_size);
			set_shortest_paths_filename(filename);
			plot_metrics(metrics, group_size);

			char ecc_filename[250] = "";
			sprintf(ecc_filename, "%sfig3_6_rand_%d_hist-ECC-%.2lf.png", figures_path, failure_size, group_size);
			set_shortest_paths_filename(ecc_filename);
			plot_eccenticities(metrics);

			delete_metrics(metrics);
		}
		// 3.7
		{
			vect *articulation_points = find_articulation_points(graph);
			articulation_point_counts->emplace_back(articulation_points->size() + 1);
			fprintf(out, "Articulation points found: %d\n", articulation_points->size());
			for (int point : (*articulation_points))
			{
				fprintf(out, "%d ", point);
			}
			fprintf(out, "\n");
			delete articulation_points;

			std::vector<std::pair<int, int>> *articulation_pairs = find_articulation_pairs(graph);
			articulation_pair_counts->emplace_back(articulation_pairs->size() + 1);
			fprintf(out, "Articulation pairs found: %d\n", articulation_pairs->size());
			for (std::pair<int, int> pair : (*articulation_pairs))
			{
				fprintf(out, "(%d, %d) ", pair.first, pair.second);
			}
			fprintf(out, "\n");
			delete articulation_pairs;
		}
		delete_graph(graph);
		delete disabled_nodes;
	}
#ifndef _DEBUG
	char cc_filename[250] = "", metrics_filename[250] = "", conn_filename[250] = "";
	sprintf(cc_filename, "%sfig3_6_rand_CC.png", figures_path);
	sprintf(metrics_filename, "%sfig3_6_rand_MET.png", figures_path);
	sprintf(conn_filename, "%sfig3_6_rand_CONN.png", figures_path);
	plt::figure_size(800, 600);
	plt::semilogx(*failure_sizes, *wcc_counts, "bo");
	plt::semilogx(*failure_sizes, *scc_counts, "ro");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Number of connected components");
	plt::title("Connected component counts on failed nodes");
	plt::save(cc_filename);
	plt::close();

	plt::figure_size(800, 600);
	plt::semilogx(*failure_sizes, *diameters, "b-");
	plt::semilogx(*failure_sizes, *avgs, "r-");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Lengths of diameters and average shortest paths");
	plt::title("Metrics on failed nodes");
	plt::save(metrics_filename);
	plt::close();

	plt::figure_size(800, 600);
	plt::loglog(*failure_sizes, *articulation_point_counts, "bo");
	plt::loglog(*failure_sizes, *articulation_pair_counts, "ro");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Number of articulation points (blue) and pairs of articulation points (red)");
	plt::title("Connectivity on failed nodes");
	plt::save(conn_filename);
	plt::close();
#endif // !_DEBUG

	delete articulation_pair_counts;
	delete articulation_point_counts;
	delete avgs;
	delete diameters;
	delete scc_counts;
	delete wcc_counts;
	clear_resistance_vars();
	fclose(out);
}
void run_attacks(const graph_t *original, vect *attack_sizes)
{
	FILE *out = fopen(output_path, "a");
	init_resistance_vars(original);

	vect *wcc_counts = new vect(), *scc_counts = new vect();
	vect *diameters = new vect();
	std::vector<double> *avgs = new std::vector<double>();
	vect *articulation_point_counts = new vect(), *articulation_pair_counts = new vect();
	for (int attack_size : (*attack_sizes))
	{
		vect *disabled_nodes = new vect(biggest_nodes, biggest_nodes + attack_size);
		graph_t *graph = create_reduced_graph(original, disabled_nodes);
		fprintf(out, "RUNNING WITH %d ATTACKED NODES: ", disabled_nodes->size());
		for (int failed_node : (*disabled_nodes)) fprintf(out, "%d ", failed_node);
		fprintf(out, "\n");

		helper int node_count = graph->node_count;
		helper vect *edges = graph->edges;
		helper vect *transposed = graph->transposed;
		// 3.2
		{
			cc_info_t *wcc_info = find_wcc(graph);
			int single_node_wcc_count = 0;
			fprintf(out, "\nWCC count: %d\n", wcc_info->cc_contents->size());
			wcc_counts->emplace_back(wcc_info->cc_contents->size());
			for (auto wcc : *(wcc_info->cc_contents))
			{
				if (wcc.second.size() == 1) continue;
				fprintf(out, "%d (size = %d) <- ", wcc.first, wcc.second.size());
				for (auto node : wcc.second) fprintf(out, "%d ", node);
				fprintf(out, "\n");
			}
			fprintf(out, "\nSingle node WCCs: ");
			for (auto scc : *(wcc_info->cc_contents))
			{
				if (scc.second.size() > 1) continue;
				fprintf(out, "%d (", scc.first);
				for (int i = 0; i < edges[scc.first].size(); ++i)
				{
					if (i > 0) fprintf(out, " ");
					fprintf(out, "%d", edges[scc.first][i]);
				}
				fprintf(out, ") ");
				++single_node_wcc_count;
			}
			fprintf(out, "(%d total)\n", single_node_wcc_count);

			cc_info_t *scc_info = find_scc(graph);
			int single_node_scc_count = 0;
			fprintf(out, "\nSCC count: %d\n", scc_info->cc_contents->size());
			scc_counts->emplace_back(scc_info->cc_contents->size());
			for (auto scc : *(scc_info->cc_contents))
			{
				if (scc.second.size() == 1) continue;
				fprintf(out, "%d (size = %d) <- ", scc.first, scc.second.size());
				for (auto node : scc.second) fprintf(out, "%d ", node);
				fprintf(out, "\n");
			}
			fprintf(out, "\nSingle node SCCs: ");
			for (auto scc : *(scc_info->cc_contents))
			{
				if (scc.second.size() > 1) continue;
				fprintf(out, "%d (", scc.first);
				for (int i = 0; i < edges[scc.first].size(); ++i)
				{
					if (i > 0) fprintf(out, " ");
					fprintf(out, "%d", edges[scc.first][i]);
				}
				fprintf(out, ") ");
				++single_node_scc_count;
			}
			fprintf(out, "(%d total)\n", single_node_scc_count);
			vect *in_components = find_in_components(scc_info);
			fprintf(out, "IN components: ");
			for (int component : (*in_components)) fprintf(out, "%d ", component);
			vect *out_components = find_out_components(scc_info);
			fprintf(out, "\nOUT components: ");
			for (int component : (*out_components)) fprintf(out, "%d ", component);
			fprintf(out, "\n");

			delete out_components;
			delete in_components;
			delete_cc_info(scc_info);
		}
		// 3.3
		{
			helper const double group_size = 4;
			char in_filename[250], out_filename[250];
			sprintf(in_filename, "%sfig3_6_att_%d_hist-IN-%.0lf.png", figures_path, attack_size, group_size);
			sprintf(out_filename, "%sfig3_6_att_%d_hist-OUT-%.0lf.png", figures_path, attack_size, group_size);
			set_in_filename(in_filename);
			set_out_filename(out_filename);
			plot_degrees(graph, group_size);
		}
		// 3.4
		{
			graph_metrics_t *metrics = find_metrics(graph);
			fprintf(out, "Global avergage distance: %.3lf, Diameter: %d\n", metrics->global_avg, metrics->diameter);
			diameters->emplace_back(metrics->diameter);
			avgs->emplace_back(metrics->global_avg);

			helper const double group_size = 0.04;
			char filename[250] = "";
			sprintf(filename, "%sfig3_6_att_%d_hist-AVG-%.2lf.png", figures_path, attack_size, group_size);
			set_shortest_paths_filename(filename);
			plot_metrics(metrics, group_size);

			char ecc_filename[250] = "";
			sprintf(ecc_filename, "%sfig3_6_att_%d_hist-ECC-%.2lf.png", figures_path, attack_size, group_size);
			set_shortest_paths_filename(ecc_filename);
			plot_eccenticities(metrics);

			delete_metrics(metrics);
		}
		// 3.7
		{
			vect *articulation_points = find_articulation_points(graph);
			articulation_point_counts->emplace_back(articulation_points->size() + 1);
			fprintf(out, "Articulation points found: %d\n", articulation_points->size());
			for (int point : (*articulation_points))
			{
				fprintf(out, "%d ", point);
			}
			fprintf(out, "\n");
			delete articulation_points;

			std::vector<std::pair<int, int>> *articulation_pairs = find_articulation_pairs(graph);
			articulation_pair_counts->emplace_back(articulation_pairs->size() + 1);
			fprintf(out, "Articulation pairs found: %d\n", articulation_pairs->size());
			for (std::pair<int, int> pair : (*articulation_pairs))
			{
				fprintf(out, "(%d, %d) ", pair.first, pair.second);
			}
			fprintf(out, "\n");
			delete articulation_pairs;
		}
		delete_graph(graph);
		delete disabled_nodes;
	}
#ifndef _DEBUG
	char cc_filename[250] = "", metrics_filename[250] = "", conn_filename[250] = "";
	sprintf(cc_filename, "%sfig3_6_att_CC.png", figures_path);
	sprintf(metrics_filename, "%sfig3_6_att_MET.png", figures_path);
	sprintf(conn_filename, "%sfig3_6_att_CONN.png", figures_path);
	plt::figure_size(800, 600);
	plt::semilogx(*attack_sizes, *wcc_counts, "bo");
	plt::semilogx(*attack_sizes, *scc_counts, "ro");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Number of connected components");
	plt::title("Connected component counts on failed nodes");
	plt::save(cc_filename);
	plt::close();

	plt::figure_size(800, 600);
	plt::semilogx(*attack_sizes, *diameters, "b-");
	plt::semilogx(*attack_sizes, *avgs, "r-");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Lengths of diameters and average shortest paths");
	plt::title("Metrics on failed nodes");
	plt::save(metrics_filename);
	plt::close();

	plt::figure_size(800, 600);
	plt::loglog(*attack_sizes, *articulation_point_counts, "bo");
	plt::loglog(*attack_sizes, *articulation_pair_counts, "ro");
	plt::xlabel("Failed nodes in graph");
	plt::ylabel("Number of articulation points (blue) and pairs of articulation points (red)");
	plt::title("Connectivity on failed nodes");
	plt::save(conn_filename);
	plt::close();
#endif // !_DEBUG

	delete articulation_pair_counts;
	delete articulation_point_counts;
	delete avgs;
	delete diameters;
	delete scc_counts;
	delete wcc_counts;
	clear_resistance_vars();
	fclose(out);
}