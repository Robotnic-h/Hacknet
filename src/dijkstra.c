#include <stdio.h>
#include "dijkstra.h"

const char *Nom[] = {"A", "B", "C", "D", "E", "F", "G"};

void Chemin(int parent[], int j){
    if (parent[j] == -1){
        printf("%s", Nom[j]);
        return;
    }
    Chemin(parent, parent[j]);
    printf(" -> %s", Nom[j]);
}

int PlusProche(int dist[], int Vu[]){
    int min = INF;
    int index = -1;

    for(int i = 0; i< NS; i++){
        if ((Vu[i] == 0) && (dist[i] <= min)){
            min = dist[i];
            index = i;
        }
    }
    return index;
}

void Init(int mat[NS], int val){
    for(int i =0; i<NS; i++){
        mat[i] = val;
    }
}

int Dji(int Mat[NS][NS], int D, int F){
    int dist[NS];
    int Vu[NS];
    int parent[NS];

    for(int i =0; i<NS; i++){
        dist[i] = INF;
        Vu[i] = 0;
        parent[i] = -1;
    }

    dist[D] = 0;

    int S;

    for(int i = 0; i < NS-1; i++){
        S = PlusProche(dist, Vu);

        if (S == -1){
            break;
        }

        Vu[S] = 1;

        for(int j = 0; j < NS; j++){
            if((Vu[j] == 0) && (Mat[S][j] != 0) && (dist[S] != INF) && (dist[S] + Mat[S][j] < dist[j])){
                dist[j] = dist[S] + Mat[S][j];
                parent[j] = S;
            }
        }

        if(S == F){
            break;
        }
    }

    printf("\n=== RESULTAT %s -> %s ===\n", Nom[D], Nom[F]);

    if (dist[F] == INF) {
        printf("Aucun chemin\n");
    } else {
        printf("Distance : %d\nChemin : ", dist[F]);
        Chemin(parent, F);
        printf("\n");
    }

    return dist[F];
}