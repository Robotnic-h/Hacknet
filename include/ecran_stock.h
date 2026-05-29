#ifndef ECRAN_STOCK_H
#define ECRAN_STOCK_H
#include "types.h"

typedef struct {
    Bouton btn_plus[NB_OBJETS];
    Bouton btn_moins[NB_OBJETS];
    Bouton btn_valider;
    Bouton btn_retour;
} EcranStock;

void    ecran_stock_init(EcranStock *es, Stock *stock);
EtatJeu ecran_stock_evenements(EcranStock *es, SDL_Event *e, Contexte *ctx, ZoneLog *log);
void    ecran_stock_rendu(EcranStock *es, Contexte *ctx, ZoneLog *log);

#endif

