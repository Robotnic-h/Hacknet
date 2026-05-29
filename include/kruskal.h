#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct DSU */
typedef struct {
    int parent[MAX_NOEUDS];
    int rang[MAX_NOEUDS];
} DSU;

/* resultat */
typedef struct {
    int cameras[MAX_NOEUDS];
    int nb_cameras;
    int cout_total;
} ResultatCouverture;

//Setup de fonctions
int cmp_arete(const void *a, const void *b);
int max_sommet(const Arete *a, int n);
int *degres(const Arete *aretes, int nb_aretes, int nb_s);
void get_index_name(int index, char name[3]);

//Algorithme principale de Kruskal modifié pour trouver un vertex cover
int kruskal_vertex_cover(Arete *aretes, int nb_aretes, int nb_sommets, int *cover);

#endif

