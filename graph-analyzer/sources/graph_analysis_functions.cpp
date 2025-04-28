#include "../headers/graph_analysis_functions.h"

static int *rep, *union_size; // find and union
static int *post_order, curr_order; // SCC finding
bool *visited; // DFS
static int *scc_index, curr_scc; // SCC creation

static void init_scc_vars(const int node_count)
{
	rep = (int *)malloc(node_count * sizeof(int));
	union_size = (int *)malloc(node_count * sizeof(int));
	post_order = (int *)malloc(node_count * sizeof(int));
	visited = (bool *)malloc(node_count * sizeof(bool));

	scc_index = (int *)malloc(node_count * sizeof(int));

	for (int i = 0; i < node_count; ++i)
	{
		rep[i] = i;
		union_size[i] = 1;
		visited[i] = false;
		scc_index[i] = -1;
	}
	curr_order = curr_scc = 0;
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
	visited[x] = true;
	helper const std::vector<int> *edges = graph->edges;
	helper const int size = edges[x].size();
	for (int i = 0; i < size; ++i)
	{
		if (!visited[edges[x][i]]) scc_dfs_forward(graph, edges[x][i]);
	}
	post_order[curr_order++] = x;
}
static void scc_dfs_backward(const graph_t *graph, const int x)
{
	visited[x] = false;
	helper const std::vector<int> *edges = graph->edges;
	helper const int size = edges[x].size();
	for (int i = 0; i < size; ++i)
	{
		helper const int neighbour = edges[x][i];
		if (visited[neighbour])
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

	for (int i = 0; i < node_count; ++i) // traverse the graph forward to calculate post orders for its nodes
	{
		if (!visited[i]) scc_dfs_forward(graph, i);
	}
	graph_t *transposed = create_transposed(graph);
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