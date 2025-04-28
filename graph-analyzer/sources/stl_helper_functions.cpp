#include "../headers/stl_helper_functions.h"

graph_t *create_transposed(const graph_t *graph)
{
	helper const int node_count = graph->node_count;
	graph_t *transposed = create_graph(node_count);

	helper const std::vector<int> *forward = graph->edges;
	helper std::vector<int> *backward = transposed->edges;
	for (int i = 0; i < node_count; ++i)
	{
		helper const int size = forward[i].size();
		for (int j = 0; j < size; ++j)
		{
			backward[forward[i][j]].emplace_back(i);
		}
	}

	return transposed;
}

void force_unique(std::vector<int> *v)
{
	std::set<int> *temp = new std::set<int>();

	helper int size = v->size();
	for (int i = 0; i < size; ++i)
	{
		temp->insert((*v)[i]);
	}
	v->clear();
	helper const std::set<int>::iterator end = temp->end();
	for (std::set<int>::iterator it = temp->begin(); it != end; ++it)
	{
		v->emplace_back(*it);
	}

	delete temp;
}