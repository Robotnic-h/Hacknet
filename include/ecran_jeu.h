#ifndef ECRAN_JEU_H
#define ECRAN_JEU_H
#include "types.h"

typedef struct {
    Bouton btn_rejouer;
    int    police_en_mouvement;
    int    timer_police;
    int    game_over;
    int    anim_gameover;
    int    chemin[MAX_NOEUDS];   /* liste de tout les neuds du chemin */
    int    nb_chemin;            /* nbrs de noeud pour le chemin */
    int police_bonus;
} EcranJeu;

void ecran_jeu_init(EcranJeu *ej, Contexte *ctx);
EtatJeu ecran_jeu_evenements(EcranJeu *ej, SDL_Event *e, Contexte *ctx);
void ecran_jeu_update(EcranJeu *ej, Contexte *ctx);
void ecran_jeu_rendu(EcranJeu *ej, Contexte *ctx);
void dessiner_game_over(EcranJeu *ej, Contexte *ctx);

#endif
