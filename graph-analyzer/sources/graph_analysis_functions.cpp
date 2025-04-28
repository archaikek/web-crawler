#include "../headers/graph_analysis_functions.h"

static int *rep, *union_size; // find and union
static int *post_order, curr_order; // SCC finding
int *visited, run_count; // DFS
static int *scc_index, curr_scc; // SCC creation

static FILE *results_file;
void pass_results_file(FILE *results)
{
	results_file = results;
}

static void init_scc_vars(const int node_count)
{
	rep = (int *)malloc(node_count * sizeof(int));
	union_size = (int *)malloc(node_count * sizeof(int));
	post_order = (int *)malloc(node_count * sizeof(int));
	visited = (int *)calloc(node_count, sizeof(int));

	scc_index = (int *)malloc(node_count * sizeof(int));

	for (int i = 0; i < node_count; ++i)
	{
		rep[i] = i;
		union_size[i] = 1;
		visited[i] = false;
		scc_index[i] = -1;
	}
	curr_order = curr_scc = run_count = 0;
}
static void clear_scc_vars()
{
	free(scc_index);

	free(visited);
	free(post_order);
	free(union_size);
	free(rep);
}

static int find(const int x)
{
	if (x == rep[x]) return x;
	return rep[x] = find(rep[x]);
}
static void onion(int x, int y)
{
	x = find(x);
	y = find(y);

	if (x == y) return;
	if (union_size[x] < union_size[y]) std::swap(x, y);

	rep[y] = rep[x];
	union_size[x] += union_size[y];
}
static void scc_dfs_forward(const graph_t *graph, const int x)
{
	visited[x] = run_count;
	helper const std::vector<int> *edges = graph->edges;
	helper const int size = edges[x].size();
	for (int i = 0; i < size; ++i)
	{
		if (visited[edges[x][i]] < run_count) scc_dfs_forward(graph, edges[x][i]);
	}
	post_order[curr_order++] = x;
}
static void scc_dfs_backward(const graph_t *graph, const int x)
{
	visited[x] = run_count;
	helper const std::vector<int> *edges = graph->edges;
	helper const int size = edges[x].size();
	for (int i = 0; i < size; ++i)
	{
		helper const int neighbour = edges[x][i];
		if (visited[neighbour] < run_count)
		{
			scc_dfs_backward(graph, neighbour);
			onion(x, neighbour);
		}
	}
}
graph_t *create_scc(const graph_t *graph, std::vector<int> **out__scc_members)
{
	helper const int node_count = graph->node_count;

	++run_count;
	for (int i = 0; i < node_count; ++i) // traverse the graph forward to calculate post orders for its nodes
	{
		if (!visited[i]) scc_dfs_forward(graph, i);
	}
	graph_t *transposed = create_transposed(graph);
	++run_count;
	for (int i = node_count - 1; i >= 0; --i) // traverse the graph backward to find which nodes belong to the same SCC
	{
		if (visited[post_order[i]]) scc_dfs_backward(transposed, post_order[i]);
	}
	delete_graph(transposed); // not needed anymore

	for (int i = 0; i < node_count; ++i) // assign indices to SCCs
	{
		if (scc_index[find(i)] == -1)
		{
			scc_index[find(i)] = curr_scc++;
		}
		scc_index[i] = scc_index[find(i)];
	}

	*out__scc_members = new std::vector<int>[curr_scc];
	for (int i = 0; i < node_count; ++i) (*out__scc_members)[scc_index[i]].emplace_back(i);

	graph_t *scc_graph = create_graph(curr_scc);
	helper const std::vector<int> *edges = graph->edges;
	helper std::vector<int> *out = scc_graph->edges;
	for (int i = 0; i < node_count; ++i)
	{
		helper const int size = edges[i].size();
		for (int j = 0; j < size; ++j)
		{
			if (scc_index[i] != scc_index[edges[i][j]])
			{
				out[scc_index[i]].emplace_back(scc_index[edges[i][j]]);
			}
		}
	}
	for (int i = 0; i < curr_scc; ++i) force_unique(out + i);

	return scc_graph;
}

std::vector<int> *get_out_components(const graph_t *graph)
{
	std::vector<int> *result = new std::vector<int>();
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	for (int i = 0; i < node_count; ++i)
	{
		if (edges[i].size() == 0) result->emplace_back(i);
	}
	return result;
}
std::vector<int> *get_in_components(const graph_t *graph)
{
	graph_t *transposed = create_transposed(graph);
	std::vector<int> *result = get_out_components(transposed);
	free(transposed);
	return result;
}
void run_3_2(graph_t *graph)
{
	fprintf(results_file, "\n\n3.2\n");
	init_scc_vars(graph->node_count);
	std::vector<int> *scc_members = NULL;
	graph_t *scc = create_scc(graph, &scc_members);

	fprintf(results_file, "WCC count: 1 (by definition)\n");
	fprintf(results_file, "SCC count: %d\n", get_node_count(scc));
	fprintf(results_file, "SCCs by nodes:\n");
	for (int i = 0; i < scc->node_count; ++i)
	{
		fprintf(results_file, "%d <-", i);
		for (int j = 0; j < scc_members[i].size(); ++j)
		{
			fprintf(results_file, " %d", scc_members[i][j]);
		}
		fprintf(results_file, "\n(%d total)\n", scc_members[i].size());
	}

	std::vector<int> *in = get_in_components(scc), *out = get_out_components(scc);
	fprintf(results_file, "IN components: \n");
	for (int i = 0; i < in->size(); ++i)
	{
		fprintf(results_file, "%d ", (*in)[i]);
	}
	fprintf(results_file, "\n(%d total)\n", in->size());
	fprintf(results_file, "OUT components: \n");
	for (int i = 0; i < out->size(); ++i)
	{
		fprintf(results_file, "%d ", (*out)[i]);
	}
	fprintf(results_file, "\n(%d total)\n", out->size());

	//print_graph(scc);

	delete in;
	delete out;
	delete[] scc_members;
	clear_scc_vars();
}


std::vector<int> *get_degrees(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	std::vector<int> *result = new std::vector<int>[node_count];

	for (int i = 0; i < node_count; ++i) result[edges[i].size()].emplace_back(i);

	return result;
}
void run_3_3(graph_t *graph)
{
	helper const int node_count = graph->node_count;
	fprintf(results_file, "\n\n3.3\n");
	graph_t *transposed = create_transposed(graph);

	std::vector<int> *out_dist = get_degrees(graph);
	fprintf(results_file, "OUT degrees:\n");
	for (int i = 0; i < node_count; ++i)
	{
		if (out_dist[i].size() > 0) fprintf(results_file, "%d nodes have OUT degree %d\n", out_dist[i].size(), i);
	}
	std::vector<int> *in_dist = get_degrees(transposed);
	fprintf(results_file, "IN degrees:\n");
	for (int i = 0; i < node_count; ++i)
	{
		if (in_dist[i].size() > 0) fprintf(results_file, "%d nodes have IN degree %d\n", in_dist[i].size(), i);
	}

	// TODO: wyznaczenie współczynników funkcji potęgowej

	delete[] in_dist;
	delete[] out_dist;
	delete_graph(transposed);
}


static void init_search_vars(const int node_count)
{
	visited = (int *)calloc(node_count, sizeof(int));
	run_count = 0;
}
static void clear_search_vars()
{
	free(visited);
}

static int *bfs(const graph_t *graph, const int start)
{
	helper const int node_count = graph->node_count;
	helper std::vector<int> *edges = graph->edges;

	int *result = (int *)malloc(node_count * sizeof(int));
	memset(result, -1, node_count * sizeof(int));
	std::queue<int> *queue = new std::queue<int>();
	queue->push(start);
	visited[start] = run_count;

	while (!queue->empty())
	{
		int curr_node = queue->front();
		queue->pop();

		helper const int size = edges[curr_node].size();
		for (int i = 0; i < size; ++i)
		{
			helper const int neighbour = edges[curr_node][i];
			if (visited[neighbour] < run_count)
			{
				visited[neighbour] = run_count;
				result[neighbour] = result[curr_node] + 1;
				queue->push(neighbour);
			}
		}
	}

	delete queue;
	return result;
}

int **shortest_paths(const graph_t *graph, double *out__avg_dist_global, int *out__diam, int **out__eccentricity, double **out__avg_dist)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	*out__avg_dist_global = 0;
	*out__diam = 0;
	*out__eccentricity = (int *)calloc(node_count, sizeof(int));
	*out__avg_dist = (double *)calloc(node_count, sizeof(double));

	int **results = (int **)malloc(node_count * sizeof(int *));
	int denominator, denominator_global = 0;
	for (int i = 0; i < node_count; ++i)
	{
		++run_count;
		helper int *result = results[i] = bfs(graph, i);
		denominator = 0;
		for (int j = 0; j < node_count; ++j)
		{
			if (result[j] < 0) continue; // ignore results for paths that can't be trespassed
			++denominator;
			(*out__avg_dist)[i] += double(result[j]);
			(*out__eccentricity)[i] = std::max((*out__eccentricity)[i], result[j]);
			*out__diam = std::max(*out__diam, result[j]);
		}
		*out__avg_dist_global += (*out__avg_dist)[i];
		(*out__avg_dist)[i] /= denominator;
		denominator_global += denominator;
	}
	*out__avg_dist_global /= denominator_global;

	return results;
}
void run_3_4(graph_t *graph)
{
	helper const int node_count = graph->node_count;
	init_search_vars(node_count);

	fprintf(results_file, "\n\n3.4\n");
	double avg_dist_global = 0;
	int diam = 0;
	int *eccentricity = NULL;
	double *avg_dist = NULL;
	int **results = shortest_paths(graph, &avg_dist_global, &diam, &eccentricity, &avg_dist);
	fprintf(results_file, "Average distance: %.4lf\n", avg_dist_global);
	fprintf(results_file, "Diameter: %d\n", diam);
	// TODO: wyznaczenie histogramów ekscentryczności i średnich odległości

	for (int i = 0; i < node_count; ++i) free(results[i]);
	free(results);
	free(avg_dist);
	free(eccentricity);
	clear_search_vars();
}

int **create_squared(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	int **squared = (int **)malloc(node_count * sizeof(int *));
	for (int i = 0; i < node_count; ++i) squared[i] = (int *)calloc(node_count, sizeof(int));

	for (int i = 0; i < node_count; ++i)
	{
		helper const int size = edges[i].size();
		for (int j = 0; j < size; ++j)
		{
			helper const int neighbour = edges[i][j];
			helper const int neigh_size = edges[neighbour].size();
			for (int k = 0; k < neigh_size; ++k)
			{
				++squared[i][edges[neighbour][k]];
			}
		}
	}

	return squared;
}
double *get_clustering_factors(const graph_t *graph, double *out__global_clustering_factor)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	int **squared = create_squared(graph);
	long long int global_num = 0, global_den = 0;

	double *clustering_factors = (double *)calloc(node_count, sizeof(double));
	for (int i = 0; i < node_count; ++i)
	{
		int numerator = 0, denominator = 0;
		helper const int size = edges[i].size();
		for (int j = 0; j < size; ++j) numerator += squared[i][edges[i][j]]; // closed paths of length 2
		for (int j = 0; j < node_count; ++j) denominator += squared[i][j]; // all paths of length 2
		global_num += numerator;
		global_den += denominator;
		clustering_factors[i] = double(numerator) / double(denominator == 0 ? 1 : denominator);
	}
	*out__global_clustering_factor = double(global_num) / double(global_den);

	for (int i = 0; i < node_count; ++i) free(squared[i]);
	free(squared);

	return clustering_factors;
}
void run_3_5(graph_t *graph)
{
	helper const int node_count = graph->node_count;
	fprintf(results_file, "\n\n3.5\n");
	double global_clustering_factor = 0;
	double *clustering_factors = get_clustering_factors(graph, &global_clustering_factor);
	fprintf(results_file, "Global clustering factor: %.4lf\n", global_clustering_factor);
	fprintf(results_file, "Local clustering factors:\n");
	for (int i = 0; i < node_count; ++i)
	{
		fprintf(results_file, "%d: %.4lf clustered\n", i, clustering_factors[i]);
	}

	free(clustering_factors);
}

static std::set<int> *get_random_numbers(const int num_count, const int max_num)
{
	std::set<int> *result = new std::set<int>();
	while (result->size() < num_count)
	{
		result->insert(std::rand() % max_num);
	}

	return result;
}
static inline bool is_in_set(const int x, const std::set<int> *set)
{
	return set->lower_bound(x) == set->end() || x == *(set->lower_bound(x));
}
static graph_t *copy_graph_with_disabled_nodes(const graph_t *graph, const std::set<int> *disabled_nodes)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	graph_t *copy = create_graph(node_count);
	helper std::vector<int> *edges_copy = copy->edges;

	for (int i = 0; i < node_count; ++i)
	{
		if (is_in_set(i, disabled_nodes)) continue;
		helper const int size = edges[i].size();
		for (int j = 0; j < size; ++j)
		{
			if (is_in_set(edges[i][j], disabled_nodes)) continue;
			edges_copy[i].emplace_back(edges[i][j]);
		}
	}

	return copy;
}
static void test_malfunction(const graph_t *graph, const double mal_size)
{
	helper const int node_count = graph->node_count;
	helper const int disabled_node_count = int(round(double(node_count) * double(mal_size) * 0.01));
	std::set<int> *disabled_nodes = get_random_numbers(disabled_node_count, node_count);

	graph_t *copy = copy_graph_with_disabled_nodes(graph, disabled_nodes);
	fprintf(results_file, "\n*** RANDOM MALFUNCTION WITH %d NODES DOWN ***\n", disabled_node_count);
	fprintf(results_file, "Disabled nodes: ");
	helper std::set<int>::iterator end = disabled_nodes->end();
	helper std::vector<int> *edges = graph->edges;
	for (std::set<int>::iterator it = disabled_nodes->begin(); it != end; ++it)
	{
		fprintf(results_file, "%d (out_deg=%d)  ", *it, edges[*it].size());
	}

	run_3_2(copy);
	run_3_3(copy);
	run_3_4(copy);

	delete_graph(copy);
}
void run_resistance_tests(const graph_t *graph, std::vector<double> *percentages)
{
	srand(2221);
	helper const int test_count = percentages->size();
	for (int i = 0; i < test_count; ++i)
	{
		test_malfunction(graph, (*percentages)[i]);
	}
}
void run_3_6(graph_t *graph)
{
	std::vector<double> *percentages = new std::vector<double>({ 0.1, 0.25, 0.5, 1, 2.5, 5, 10 });
	run_resistance_tests(graph, percentages);
	delete percentages;
}