#pragma once
#include "structs.h"
#include "stl_helper_functions.h"

vect *find_articulation_points(const graph_t *graph);
std::vector<std::pair<int, int>> *find_articulation_pairs(const graph_t *graph);
