#include "../headers/structs.h"

graph_t *create_graph(const int node_count)
{
	graph_t *graph = (graph_t *)malloc(sizeof(graph_t));

	graph->node_count = node_count;
	graph->edges = new vect[node_count];
	graph->transposed = new vect[node_count];

	return graph;
}
graph_t *create_reduced_graph(const graph_t *original, vect *disabled_nodes)
{
	helper const int node_count = original->node_count;
	helper const vect *edges = original->edges;
	helper const int disabled_count = disabled_nodes->size();
	helper const int *disabled = &((*disabled_nodes)[0]);
	graph_t *graph = create_graph(node_count - disabled_count);
	helper vect *new_edges = graph->edges;

	int *translation = (int *)malloc(node_count * sizeof(int));
	sort(disabled_nodes->begin(), disabled_nodes->end());
	int pos = 0;
	for (int i = 0; i < node_count; ++i)
	{
		if (i == disabled[pos])
		{
			translation[i] = -1;
			++pos;
			continue;
		}
		translation[i] = i - pos;
	}

	for (int i = 0; i < node_count; ++i)
	{
		helper const int index = translation[i];
		if (index < 0) continue;
		helper const int size = edges[index].size(); if (size == 0) continue;
		helper const int *neighbours = &(edges[index][0]);
		for (int j = 0; j < size; ++j)
		{
			if (translation[neighbours[j]] < 0) continue;
			add_edge(graph, index, translation[neighbours[j]]);
		}
	}

	free(translation);
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
void clear_node(graph_t *graph, const int node)
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

	for (int i = 0; i < node_count; ++i)
	{
		result += edges[i].size();
	}

	return result;
}