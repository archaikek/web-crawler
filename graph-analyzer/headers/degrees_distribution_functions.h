#pragma once

#include "structs.h"
#include "stl_helper_functions.h"

vect *get_in_degrees(const graph_t *graph, const int group_size);
vect *get_out_degrees(const graph_t *graph, const int group_size);

void plot_degrees_hist(const vect *in_degs, const vect *out_degs, const int group_size);
