#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

#define NS 7
#define INF INT_MAX

extern const char *Nom[];

void Chemin(int parent[], int j);
int PlusProche(int dist[], int Vu[]);
void Init(int mat[NS], int val);
int Dji(int Mat[NS][NS], int D, int F);

#endif