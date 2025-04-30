#include "../headers/structs.h"

graph_t *create_graph(const int node_count)
{
	graph_t *graph = (graph_t *)malloc(sizeof(graph_t));

	graph->node_count = node_count;
	graph->edges = new vect[node_count];
	graph->transposed = new vect[node_count];

	return graph;
}
void delete_graph(graph_t *graph)
{
	delete[] graph->transposed;
	delete[] graph->edges;
	free(graph);
}

std::pair<vect, vect> *get_node_copy(const graph_t *graph, const int node)
{
	std::pair<vect, vect> *result = new std::pair<vect, vect>(graph->edges[node], graph->transposed[node]);
	return result;
}
void remove_node(graph_t *graph, const int node)
{
	helper vect *edges = graph->edges;
	helper vect *transposed = graph->transposed;
	helper const int e_size = edges[node].size();
	helper const int t_size = transposed[node].size();

	// remove all transposed connections to node
	for (int i = 0; i < e_size; ++i)
	{
		helper const int neighbour = edges[node][i];
		helper const int size = transposed[neighbour].size();
		for (int j = 0; j < size; ++j)
		{
			if (transposed[neighbour][j] == node)
			{
				std::swap(transposed[neighbour][j], transposed[neighbour].back());
				transposed[neighbour].pop_back();
			}
		}
	}
	// remove all normal connections to node
	for (int i = 0; i < t_size; ++i)
	{
		helper const int neighbour = transposed[node][i];
		helper const int size = edges[neighbour].size();
		for (int j = 0; j < size; ++j)
		{
			if (edges[neighbour][j] == node)
			{
				std::swap(edges[neighbour][j], edges[neighbour].back());
				edges[neighbour].pop_back();
			}
		}
	}
}

//inline void add_edge(graph_t *graph, const int src, const int dst);

//inline int get_node_count(const graph_t *graph);
int get_edge_count(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	
	int result = 0;

	for (int i = 0; i < node_count; ++i) result += edges[i].size();

	return result;
}