#include "../headers/connectivity_functions.h"

static int *visited, *lowpoint, *depth, run_count;
static void init_connectivity_vars(const int node_count)
{
	visited = (int *)calloc(node_count, sizeof(int));
	lowpoint = (int *)malloc(node_count * sizeof(int));
	depth = (int *)malloc(node_count * sizeof(int));
	run_count = 0;
}
static void clear_connectivity_vars()
{
	free(depth);
	free(lowpoint);
	free(visited);
}

static void get_articulation_points(const vect *edges, const int node, const int parent, const int dep, vect *results)
{
	visited[node] = run_count;
	lowpoint[node] = depth[node] = dep;
	int child_count = 0;
	bool is_aritculation_point = false;

	helper const int size = edges[node].size();
	helper const int *neighbours = size == 0 ? NULL : &(edges[node][0]);
	for (int i = 0; i < size; ++i)
	{
		helper const int neighbour = neighbours[i];
		if (visited[neighbour] < run_count)
		{
			get_articulation_points(edges, neighbour, node, dep + 1, results);
			++child_count;
			is_aritculation_point = (lowpoint[neighbour] >= dep);
			lowpoint[node] = std::min(lowpoint[node], lowpoint[neighbour]);
		}
		else if (neighbour != parent)
		{
			lowpoint[node] = std::min(lowpoint[node], depth[neighbour]);
		}
	}
	if ((parent >= 0 && is_aritculation_point) || (parent < 0 && child_count > 1)) results->emplace_back(node);
}
vect *find_articulation_points(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	vect *results = new vect();
	init_connectivity_vars(node_count);

	++run_count;
	for (int i = 0; i < node_count; ++i)
	{
		if (visited[i] < run_count) get_articulation_points(edges, i, -1, 0, results);
	}

	clear_connectivity_vars();
	return results;
}
std::vector<std::pair<int, int>> *find_articulation_pairs(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	std::vector<std::pair<int, int>> *results = new std::vector<std::pair<int, int>>();
	init_connectivity_vars(node_count);

	vect *curr_node = new vect(1), *partial_results = new vect();
	for (int i = 0; i < node_count; ++i)
	{
		(*curr_node)[0] = i;
		partial_results->clear();
		graph_t *new_graph = create_reduced_graph(graph, curr_node);

		++run_count;
		for (int j = 0; j < node_count - 1; ++j)
		{
			if (visited[j] < run_count) get_articulation_points(new_graph->edges, j, -1, 0, partial_results);
		}
		
		helper const int size = partial_results->size();
		helper const int *partial_points = size == 0 ? NULL : &((*partial_results)[0]);
		for (int j = 0; j < size; ++j) if (partial_points[j] < i) results->emplace_back(std::make_pair(partial_points[j], i));
		delete_graph(new_graph);
	}

	delete partial_results;
	delete curr_node;
	clear_connectivity_vars();
	return results;
}