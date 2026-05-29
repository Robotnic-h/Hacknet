#ifndef ECRAN_RESULTAT_H
#define ECRAN_RESULTAT_H
#include "types.h"

typedef struct { int dummy; } EcranResultat;
void    ecran_resultat_init(EcranResultat *er, Stock *stock,
                            Bouton *btn_suite, Bouton *btn_retour);
EtatJeu ecran_resultat_evenements(Bouton *btn_suite, Bouton *btn_retour, SDL_Event *e);
void    ecran_resultat_rendu(Contexte *ctx, Bouton *btn_suite, Bouton *btn_retour);

#endif
