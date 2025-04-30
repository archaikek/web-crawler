#pragma once

#include "structs.h"
#include "stl_helper_functions.h"

typedef struct
{
	std::map<int, vect> *cc_contents;
	graph_t *cc_graph, *original;
	vect *reps;
} cc_info_t;

void delete_cc_info(cc_info_t *info);

cc_info_t *find_wcc(graph_t *graph);
cc_info_t *find_scc(graph_t *graph);

vect *find_in_components(const cc_info_t *info);
vect *find_out_components(const cc_info_t *info);