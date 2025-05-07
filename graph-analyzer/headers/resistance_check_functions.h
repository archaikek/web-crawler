#pragma once

#include "structs.h"
#include "stl_helper_functions.h"
#include "connected_components.h"
#include "degrees_distribution_functions.h"
#include "shortest_paths_functions.h"
#include "clustering_functions.h"
#include "connectivity_functions.h"

void run_random_failures(const graph_t *original, vect *failure_sizes);
void run_attacks(const graph_t *original, vect *attack_sizes);