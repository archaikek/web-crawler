#pragma once

#include "structs.h"
#include "stl_helper_functions.h"

double *get_pagerank(const graph_t *graph, const double damping, const double precision);
void plot_pagerank(std::vector<double> *pagerank, const double damping, const double group_size);

void set_pagerank_filename(const char *new_filename);