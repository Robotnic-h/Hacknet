#ifndef ECRAN_CAMERAS_H
#define ECRAN_CAMERAS_H
#include "types.h"

typedef struct {
    Bouton btn_suite;
    Bouton btn_retour;
    int    nb_cameras;
    int    timer_pulse;
} EcranCameras;

void    ecran_cameras_init(EcranCameras *ec, Graphe *graphe);
EtatJeu ecran_cameras_evenements(EcranCameras *ec, SDL_Event *e);
void    ecran_cameras_update(EcranCameras *ec);
void    ecran_cameras_rendu(EcranCameras *ec, Contexte *ctx);
#endif
