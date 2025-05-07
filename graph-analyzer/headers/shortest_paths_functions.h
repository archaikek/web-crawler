#pragma once

#include "structs.h"
#include "stl_helper_functions.h"

typedef struct
{
	double global_avg;
	int diameter;
	vect *eccentricities;
	std::vector<double> *avg;
} graph_metrics_t;

void delete_metrics(graph_metrics_t *metrics);

graph_metrics_t *find_metrics(const graph_t *graph);

void plot_metrics(const graph_metrics_t *metrics, const double group_size);
void plot_eccenticities(const graph_metrics_t *metrics);

void set_shortest_paths_filename(const char *new_filename);