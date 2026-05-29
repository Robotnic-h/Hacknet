#ifndef ASTAR_H
#define ASTAR_H

#include "types.h"
#define MAX_NODES 100

int astar_graphe(Graphe *g, int src, int dst, int *path);

#endif