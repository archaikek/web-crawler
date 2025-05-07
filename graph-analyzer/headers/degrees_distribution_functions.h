#pragma once

#include "structs.h"
#include "stl_helper_functions.h"

vect *get_in_degrees(const graph_t *graph);
vect *get_out_degrees(const graph_t *graph);

void plot_degrees(const graph_t *graph, const double group_size);

void set_in_filename(const char *filename);
void set_out_filename(const char *filename);