#include "../headers/shortest_paths_functions.h"

static unsigned int **shortest_paths;
static int *visited, run_count;
static void init_shortest_paths_vars(const int node_count)
{
	shortest_paths = (unsigned int **)malloc(node_count * sizeof(unsigned int *));
	for (int i = 0; i < node_count; ++i)
	{
		shortest_paths[i] = (unsigned int  *)malloc(node_count * sizeof(unsigned));
		memset(shortest_paths[i], -1, node_count * sizeof(unsigned int));
	}
	visited = (int *)calloc(node_count, sizeof(int));
	run_count = 0;
}
static void clear_shortest_paths_vars(const int node_count)
{
	free(visited);
	for (int i = 0; i < node_count; ++i) free(shortest_paths[i]);
	free(shortest_paths);
}

static graph_metrics_t *create_metrics()
{
	graph_metrics_t *metrics = (graph_metrics_t *)malloc(sizeof(graph_metrics_t));
	metrics->eccentricities = new vect();
	metrics->avg = new std::vector<double>();
	return metrics;
}
void delete_metrics(graph_metrics_t *metrics)
{
	delete metrics->avg;
	delete metrics->eccentricities;
	free(metrics);
}

static void bfs(const vect *edges, const int node)
{
	std::queue<int> *pending_nodes = new std::queue<int>();
	pending_nodes->push(node);
	visited[node] = run_count;

	helper unsigned int *distances = shortest_paths[node];
	distances[node] = 0;
	while (!pending_nodes->empty())
	{
		const int curr_node = pending_nodes->front();
		pending_nodes->pop();

		helper const int size = edges[curr_node].size(); if (size == 0) continue;
		helper const int *neighbours = &(edges[curr_node][0]);
		for (int i = 0; i < size; ++i)
		{
			helper const int neighbour = neighbours[i];
			if (visited[neighbour] < run_count)
			{
				visited[neighbour] = run_count;
				pending_nodes->push(neighbour);
				distances[neighbour] = distances[curr_node] + 1;
			}
		}
	}

	delete pending_nodes;
}
static void find_shortest_paths(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;

	for (int i = 0; i < node_count; ++i)
	{
		++run_count;
		bfs(edges, i);
	}
}

graph_metrics_t *find_metrics(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	graph_metrics_t *metrics = create_metrics();
	init_shortest_paths_vars(node_count);

	find_shortest_paths(graph);
	int global_paths = 0, global_max = 0;
	double global_dist = 0;
	helper vect *eccenticities = metrics->eccentricities;
	helper std::vector<double> *avg = metrics->avg;
	for (int i = 0; i < node_count; ++i)
	{
		int paths = 0, max_dist = 0;
		double dist = 0;
		helper const unsigned int *distances = shortest_paths[i];
		for (int j = 0; j < node_count; ++j)
		{
			if (distances[j] > node_count) continue; // node unreachable
			++paths;
			dist += distances[j];
			max_dist = std::max(max_dist, int(distances[j]));
		}
		global_dist += dist;
		global_paths += paths;
		global_max = std::max(global_max, max_dist);
		eccenticities->emplace_back(max_dist);
		avg->emplace_back(dist / paths);
	}
	metrics->diameter = global_max;
	metrics->global_avg = global_dist / global_paths;

	clear_shortest_paths_vars(node_count);
	return metrics;
}

void plot_metrics(const graph_metrics_t *metrics, const double group_size)
{
	helper std::vector<double> *avg = metrics->avg;
	std::pair<std::vector<double>, std::vector<int>> axes = make_histogram(avg, group_size);
	std::pair<double, double> coeff = regress_linear(&(axes.first), &(axes.second));
	std::vector<double> *line = make_line(&(axes.first), coeff, calculate_linear_fun);

	char filename[250] = "";
	sprintf(filename, "%s%s%.2lf.png", figures_path, "fig3_4_hist-AVG-", group_size);

#ifndef _DEBUG
	char title[250];
	plt::figure_size(800, 600);
	plt::plot(axes.first, axes.second, "r-");
	plt::plot(axes.first, *line, "y--");
	plt::xlabel("Average distance");
	plt::ylabel("Number of nodes with given distance");
	sprintf(title, "Distribution of average distances in graph (grouped by %.2lf)\napprox. %.3lfx + %.3lf\n", group_size, coeff.first, coeff.second);
	plt::title(title);
	plt::save(filename);
	plt::close();
	printf("%s", title);
#endif
}
void plot_eccenticities(const graph_metrics_t *metrics)
{
	helper std::vector<int> *eccentricities = metrics->eccentricities;
	std::pair<std::vector<double>, std::vector<int>> axes = make_histogram(eccentricities, 1);
	//std::pair<double, double> coeff = regress_linear(&(axes.first), &(axes.second));
	//std::vector<double> *line = make_line(&(axes.first), coeff, calculate_linear_fun);

	char filename[250] = "";
	sprintf(filename, "%s%s.png", figures_path, "fig3_4_hist-ECC");

#ifndef _DEBUG
	char title[250];
	plt::figure_size(800, 600);
	plt::plot(axes.first, axes.second, "bo");
	for (int i = 0; i < axes.first.size(); ++i)
	{
		char annotation[20];
		sprintf(annotation, "%d", axes.second[i]);
		plt::annotate(annotation, axes.first[i] - 0.1, axes.second[i] + 40);
	}
	//plt::plot(axes.first, *line, "y-");
	plt::xlabel("Eccentricity");
	plt::ylabel("Number of nodes with given eccentricity");
	sprintf(title, "Distribution of eccentricities in graph");
	plt::title(title);
	plt::save(filename);
	plt::close();
#endif
}