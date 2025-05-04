#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef _DEBUG
#include "matplotlib-cpp-master/matplotlibcpp.h"
namespace plt = matplotlibcpp;
#endif
#include <vector>
#include <utility>
#include <map>
#include <unordered_set>
#include <math.h>
#include <cstring>
#include <queue>
#include <algorithm>

#define helper
#define sq(x) ((x) * (x))
#define INF ((1<<31) - 1)

const char input_path[] = "../graph.in";
const char output_path[] = "../results.out";
const char figures_path[] = "../figures/";