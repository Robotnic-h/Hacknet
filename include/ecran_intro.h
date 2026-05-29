#ifndef ECRAN_INTRO_H
#define ECRAN_INTRO_H

#include "types.h"

typedef struct {
    int     ligne_courante;             /* ligne du briefing act     */
    int     char_courant;               /* cract en cours d'affichage  */
    int     timer_typo;                 /* compteur pour faire un effet typo       */
    char    texte_affiche[16][256];     /* text afficher  */
    Bouton  btn_villes[3];              /* boutons ville    */
    Bouton  btn_go;                     /* bouton "Infiltrer"               */
    int     ville_selectee;             /* -1 = aucune, 0 a 2 = ville         */
} EcranIntro;

void    ecran_intro_init(EcranIntro *ei);
EtatJeu ecran_intro_evenements(EcranIntro *ei, SDL_Event *e, Contexte *ctx);
void    ecran_intro_update(EcranIntro *ei);
void    ecran_intro_rendu(EcranIntro *ei, Contexte *ctx);

#endif
