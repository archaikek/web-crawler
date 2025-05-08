#include "../headers/connected_components.h"

static cc_info_t *create_cc_info()
{
	cc_info_t *info = (cc_info_t *)malloc(sizeof(cc_info_t));
	info->cc_contents = new std::map<int, vect>();
	info->cc_graph = NULL;
	info->reps = new vect();
	return info;
}
void delete_cc_info(cc_info_t *info)
{
	delete info->reps;
	delete info->cc_contents;
	if (info->cc_graph != NULL) delete_graph(info->cc_graph);
	free(info);
}

// standard find and union functions
static int find(int *rep, const int node)
{
	if (node == rep[node]) return node;
	return rep[node] = find(rep, rep[node]);
}
static void onion(int *rep, int *sizes, int node_x, int node_y)
{
	node_x = find(rep, node_x);
	node_y = find(rep, node_y);
	if (node_x == node_y) return;

	if (sizes[node_x] < sizes[node_y]) std::swap(node_x, node_y);
	sizes[node_x] += sizes[node_y];
	rep[node_y] = rep[node_x];
}
// reconnects connected components representatives to the lowest-indexed node of that connected component
static void reconnect_to_lowest(int *rep, int *sizes, const int node_count)
{
	for (int i = 0; i < node_count; ++i)
	{
		if (i < find(rep, i)) // the representative of i's CC has higher index than i - should be reconnected
		{
			if (sizes != NULL) sizes[i] = sizes[find(rep, i)]; // sizes reconnection is optional, as they're usually not needed after connections are made anyway
			rep[find(rep, i)] = i;
			rep[i] = i;
		}
		rep[i] = find(rep, rep[i]);
	}
}

// weakly connected components tables
static int *wcc_rep, *wcc_sizes;
// strongly conntected components tables
static int *indices, *lowlink, *scc_rep, index;
static bool *on_stack;
vect *stack;

static void init_wcc_vars(const int node_count)
{
	wcc_rep =	(int *)malloc(node_count * sizeof(int));
	wcc_sizes = (int *)malloc(node_count * sizeof(int));
	for (int i = 0; i < node_count; ++i)
	{
		wcc_rep[i] = i;
		wcc_sizes[i] = 1;
	}
}
static void clear_wcc_vars()
{
	free(wcc_sizes);
	free(wcc_rep);
}
static void init_scc_vars(const int node_count)
{
	indices =	(int *)malloc(node_count * sizeof(int));
	lowlink =	(int *)malloc(node_count * sizeof(int));
	scc_rep =	(int *)malloc(node_count * sizeof(int));
	on_stack =	(bool *)calloc(node_count, sizeof(bool));
	stack = new vect();

	memset(indices, -1, node_count * sizeof(int));
	memset(scc_rep, -1, node_count * sizeof(int));

	index = 0;
}
static void clear_scc_vars()
{
	delete stack;
	free(on_stack);
	free(scc_rep);
	free(lowlink);
	free(indices);
}

// standard DFS with find and union connections
static void weakconnect(const vect *edges, const int node)
{
	helper const int size = edges[node].size();	if (size == 0) return;
	helper const int *neighbours = &(edges[node][0]);
	for (int i = 0; i < size; ++i) onion(wcc_rep, wcc_sizes, node, neighbours[i]);
}
cc_info_t *find_wcc(graph_t *graph)
{
	cc_info_t *wcc_info = create_cc_info();
	wcc_info->original = graph;

	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	helper std::map<int, vect> *contents = wcc_info->cc_contents;

	init_wcc_vars(node_count);
	for (int i = 0; i < node_count; ++i)
	{
		weakconnect(edges, i);
	}
	reconnect_to_lowest(wcc_rep, wcc_sizes, node_count);
	for (int i = 0; i < node_count; ++i)
	{
		(*contents)[wcc_rep[i]].emplace_back(i);
	}

	clear_wcc_vars();
	return wcc_info;
}

// Tarjan's algorithm for finding SCCs in graph
static void strongconnect(const vect *edges, const int node)
{
	indices[node] = lowlink[node] = index++;
	stack->emplace_back(node);
	on_stack[node] = true;

	helper const int size = edges[node].size();
	if (size > 0)
	{
		helper const int *neighbours = &(edges[node][0]);
		for (int i = 0; i < size; ++i)
		{
			helper const int neighbour = neighbours[i];
			if (indices[neighbour] < 0) // neighbour not visited yet
			{
				strongconnect(edges, neighbour);
				lowlink[node] = std::min(lowlink[node], lowlink[neighbour]);
			}
			else if (on_stack[neighbour]) // neighbour on stack must belong to the same SCC
			{
				lowlink[node] = std::min(lowlink[node], indices[neighbour]);
			} // otherwise node->neighbour would connect to another, already discovered SCC, and should be ignored
		}
	}
	if (lowlink[node] == indices[node]) // no earlier node in this SCC has been found, otherwise lowlink[node] would point to it
	{
		for (;;)
		{
			helper const int top = stack->back();
			stack->pop_back();
			on_stack[top] = false;
			scc_rep[top] = node;
			if (top == node) break;
		}
	}
}
static void make_scc_graph(cc_info_t *info)
{
	helper std::map<int, vect> *cc_contents = info->cc_contents;
	helper const int scc_count = cc_contents->size();
	info->cc_graph = create_graph(scc_count);

	helper const std::map<int, vect>::iterator end = cc_contents->end();
	helper vect *reps = info->reps;
	std::map<int, int> *temp = new std::map<int, int>();
	for (std::map<int, vect>::iterator it = cc_contents->begin(); it != end; ++it)
	{
		reps->emplace_back(it->first);
		(*temp)[it->first] = reps->size() - 1;
	}

	helper graph_t *cc_graph = info->cc_graph;
	helper const vect *edges = info->original->edges;
	helper const vect *transposed = info->original->transposed;
	for (int i = 0; i < scc_count; ++i)
	{
		helper const vect *nodes = &((*cc_contents)[(*reps)[i]]);
		helper const int node_count = nodes->size();
		for (int j = 0; j < node_count; ++j)
		{
			helper const int node = (*nodes)[j];
			helper const int e_size = edges[node].size();
			helper const int t_size = transposed[node].size();
			if (e_size > 0)
			{
				helper const int *e_neighbours = &(edges[node][0]);
				for (int k = 0; k < e_size; ++k) if (i != scc_rep[(*temp)[e_neighbours[k]]]) add_edge(cc_graph, i, scc_rep[(*temp)[e_neighbours[k]]]);

			}
			if (t_size > 0)
			{
				helper const int *t_neighbours = &(transposed[node][0]);
				for (int k = 0; k < t_size; ++k) if (i != scc_rep[(*temp)[t_neighbours[k]]]) add_edge(cc_graph, i, scc_rep[(*temp)[t_neighbours[k]]]);
			}
		}
	}
	for (int i = 0; i < scc_count; ++i)
	{
		remove_duplicates(cc_graph->edges + i);
		remove_duplicates(cc_graph->transposed + i);
	}
	delete temp;
}
cc_info_t *find_scc(graph_t *graph)
{
	cc_info_t *scc_info = create_cc_info();
	scc_info->original = graph;

	helper const int node_count = graph->node_count;
	helper const vect *edges = graph->edges;
	helper std::map<int, vect> *contents = scc_info->cc_contents;

	init_scc_vars(node_count);
	for (int i = 0; i < node_count; ++i)
	{
		if (indices[i] < 0) strongconnect(edges, i);
	}
	//for (int i = 0; i < node_count; ++i) printf("??? scc_rep[%d] = %d\n", i, scc_rep[i]);
	reconnect_to_lowest(scc_rep, NULL, node_count);
	for (int i = 0; i < node_count; ++i)
	{
		(*contents)[scc_rep[i]].emplace_back(i);
	}

	make_scc_graph(scc_info);

	clear_scc_vars();
	return scc_info;
}

vect *find_in_components(const cc_info_t *info)
{
	vect *result = new vect();

	helper const int cc_count = info->cc_graph->node_count;
	helper const vect *transposed = info->cc_graph->transposed;
	for (int i = 0; i < cc_count; ++i)
	{
		if (transposed[i].empty()) result->emplace_back(i);
	}

	return result;
}
vect *find_out_components(const cc_info_t *info)
{
	vect *result = new vect();

	helper const int cc_count = info->cc_graph->node_count;
	helper const vect *edges = info->cc_graph->edges;
	for (int i = 0; i < cc_count; ++i)
	{
		if (edges[i].empty()) result->emplace_back(i);
	}

	return result;
}