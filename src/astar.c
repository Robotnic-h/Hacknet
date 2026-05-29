#include <math.h>
#include <float.h>
#include <string.h>
#include "astar.h"

typedef struct {
    double g, h, f;
    int    parent, open, closed;
} NodeInfo;

static double heuristic(Graphe *g, int u, int v) {
    double dx = g->noeuds[u].x - g->noeuds[v].x;
    double dy = g->noeuds[u].y - g->noeuds[v].y;
    return sqrt(dx * dx + dy * dy);
}

/* utilise g->mat et coordonnees des neuds */
int astar_graphe(Graphe *g, int src, int dst, int *path) {
    int n = g->nb_noeuds;
    NodeInfo info[MAX_NODES];

    for (int i = 0; i < n; i++) {
        info[i].g      = DBL_MAX;
        info[i].h      = heuristic(g, i, dst);
        info[i].f      = DBL_MAX;
        info[i].parent = -1;
        info[i].open   = 0;
        info[i].closed = 0;
    }
    info[src].g    = 0.0;
    info[src].f    = info[src].h;
    info[src].open = 1;

    while (1) {
        /* NNoued ouvert avec f min */
        int current = -1;
        double best_f = DBL_MAX;
        for (int i = 0; i < n; i++)
            if (info[i].open && info[i].f < best_f)
                { best_f = info[i].f; current = i; }

        if (current == -1) return -1;  /* si pas de chemin */
        if (current == dst) break;

        info[current].open   = 0;
        info[current].closed = 1;

        for (int nb = 0; nb < n; nb++) {
            double w = (double)g->mat[current][nb];
            if (w <= 0.0 || info[nb].closed) continue;

            double tg = info[current].g + w;
            if (tg < info[nb].g) {
                info[nb].parent = current;
                info[nb].g      = tg;
                info[nb].f      = tg + info[nb].h;
                info[nb].open   = 1;
            }
        }
    }

    /* construction chemin */
    int tmp[MAX_NODES], len = 0, cur = dst;
    while (cur != -1) { tmp[len++] = cur; cur = info[cur].parent; }
    for (int i = 0; i < len; i++) path[i] = tmp[len - 1 - i];
    return len;
}