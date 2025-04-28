#include "../headers/graph_analysis_functions.h"

static int *rep, *union_size; // find and union
static int *post_order, curr_order; // SCC finding
int *visited, run_count; // DFS
static int *scc_index, curr_scc; // SCC creation

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
	init_scc_vars(node_count);

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

	clear_scc_vars();
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


std::vector<int> *get_degrees(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const std::vector<int> *edges = graph->edges;
	std::vector<int> *result = new std::vector<int>[node_count];

	for (int i = 0; i < node_count; ++i) result[edges[i].size()].emplace_back(i);

	return result;
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

	init_search_vars(node_count);
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

	clear_search_vars();
	return results;
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