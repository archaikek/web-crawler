#pragma once
#include "stl_helper_functions.h"
#include "structs.h"

typedef struct
{
	double global;
	double *local;
	int node_count;
} cluster_info_t;

void delete_cluster_info(cluster_info_t *info);

cluster_info_t *find_clustering(const graph_t *graph);

void plot_clustering(const cluster_info_t *info, const double group_size);