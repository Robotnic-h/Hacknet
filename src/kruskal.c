#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kruskal.h"

int cmp_arete(const void *a, const void *b) //Compare deux aretes par poids pour les trier
{
    return ((Arete *)a)->poids - ((Arete *)b)->poids;
}

int max_sommet(const Arete *a, int n) // Trouve le nombre de sommets en cherchant le plus grand index de sommet dans les arêtes, puis en ajoutant 1 (car les indices commencent à 0)
{
    int m = 0;
    for (int i = 0; i < n; i++) {
        if (a[i].id_a > m) m = a[i].id_a;
        if (a[i].id_b > m) m = a[i].id_b;
    }
    return m + 1;
}

int *degres(const Arete *a, int n, int nb_s) //Compte le nombre de fois que chaque sommet apparaît dans les arêtes pour avoir son degré
{
    int *d = calloc(nb_s, sizeof(int));
    for (int i = 0; i < n; i++) {
        d[a[i].id_a]++;
        d[a[i].id_b]++;
    }
    return d;
}

void get_index_name(int index, char name[3]) //fonction qui sert à avoir l'index en lettres pour le visuel
{
    int n = index + 1;
    int len = 0;
    char buf[8];

    while (n > 0) {
        int rem = (n - 1) % 26;
        buf[len++] = 'A' + rem;
        n = (n - 1) / 26;
    }

    for (int i = 0; i < len; i++)
        name[i] = buf[len - 1 - i];
    name[len] = '\0';
}

/*
    Algorithme kruskal modifié en version gloutonne 
    pour trouver un vertex cover (couvrement minimum  de sommets) 
    à partir d'une liste d'arêtes. 
*/
int kruskal_vertex_cover(Arete *aretes, int nb_aretes, int nb_sommets, int *cover) 
{
    // Copie triée par poids croissant
    Arete *tri = malloc(nb_aretes * sizeof(Arete));
    memcpy(tri, aretes, nb_aretes * sizeof(Arete));
    qsort(tri, nb_aretes, sizeof(Arete), cmp_arete);

    int *deg = degres(tri, nb_aretes, nb_sommets); // Calcul des degrés
    int taille = 0;

    for (int i = 0; i < nb_aretes; i++) // Pour chaque arete
    {
        int u = tri[i].id_a;
        int v = tri[i].id_b;

        if (cover[u] || cover[v]) //skip car l'arête est déjà couverte
            continue;

        /* Les deux sommets sont pas dans le couvrement cover : il faut en prendre au moins un.
           On prend celui de plus grand degré pour couvrir un maximum d'arêtes. */
        if (deg[u] >= deg[v]) 
        {
            cover[u] = 1; taille++;
            /* Si l'arête n'est toujours pas couverte 
            (u==v impossible, donc
            ce cas n'arrive jamais, mais par symétrie on garde le check) */
            if (!cover[u] && !cover[v]) { cover[v] = 1; taille++; }
        } 
        else 
        {
            cover[v] = 1; taille++;
            if (!cover[u] && !cover[v]) { cover[u] = 1; taille++; }
        }
    }

    free(tri); //on libère toutes les allocations dynamiques
    free(deg);
    return taille;
}