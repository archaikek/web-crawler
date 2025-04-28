#pragma once
#include "structs.h"
#include <set>

graph_t *create_transposed(const graph_t *graph);

void force_unique(std::vector<int> *v);