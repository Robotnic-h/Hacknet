#include <stdio.h>
#include <string.h>

#include "types.h"
#include "rendu.h"
#include "ecran_intro.h"
#include "ecran_stock.h"
#include "ecran_resultat.h"
#include "ecran_cameras.h"
#include "ecran_jeu.h"
#include "Graph_LAVAL.h"
#include "Graph_PARIS.h"
#include "Graph_AGEN.h"

#define FPS_CIBLE    60
#define MS_PAR_FRAME (1000 / FPS_CIBLE)

//charge graph selon ville choisi
static void charger_graphe(Contexte *ctx) {
    switch (ctx->ville_choisie) {
        case 0: init_graphe_PARIS(&ctx->graphe); break;
        case 1: init_graphe_LAVAL(&ctx->graphe); break;
        case 2: init_graphe_AGEN(&ctx->graphe);  break;
    }
}

int main(void) {

    Contexte ctx;
    memset(&ctx, 0, sizeof(Contexte));
    ctx.etat = ETAT_INTRO;

    if (!rendu_init(&ctx)) {
        fprintf(stderr, "[ERREUR] Impossible d'initialiser SDL2\n");
        return 1;
    }

    EcranIntro    ei;  ecran_intro_init(&ei);
    EcranStock    es;  ecran_stock_init(&es, &ctx.stock);
    EcranResultat er;
    Bouton        btn_res_suite, btn_res_retour;
    EcranCameras  ec;
    EcranJeu      ej;

    memset(&ctx.log, 0, sizeof(ZoneLog));
    log_ajouter(&ctx.log, "> HACKNET v2.4 demarre.", 0);
    log_ajouter(&ctx.log, "> En attente de connexion...", 1);

    SDL_Event event;
    Uint32    temps_debut;
    EtatJeu   etat_precedent = ETAT_QUITTER;

    while (ctx.etat != ETAT_QUITTER) {

        temps_debut = SDL_GetTicks();

        /* transition de init a etat suivant*/
        if (ctx.etat != etat_precedent) {
            switch (ctx.etat) {
                case ETAT_STOCK:
                    /* charge graph des qu'on entre dans stock */
                    charger_graphe(&ctx);
                    ecran_cameras_init(&ec, &ctx.graphe);
                    break;
                case ETAT_RESULTAT:
                    ecran_resultat_init(&er, &ctx.stock,
                                        &btn_res_suite, &btn_res_retour);
                    break;
                case ETAT_JEU:
                    ecran_jeu_init(&ej, &ctx);
                    break;
                case ETAT_GAME_OVER:
                    break;
                default:
                    break;
            }
            etat_precedent = ctx.etat;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                ctx.etat = ETAT_QUITTER; break;
            }
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE) {
                ctx.etat = ETAT_QUITTER; break;
            }

            EtatJeu nouvel_etat = ctx.etat;
            switch (ctx.etat) {
                case ETAT_INTRO:
                    nouvel_etat = ecran_intro_evenements(&ei, &event, &ctx);
                    break;
                case ETAT_STOCK:
                    nouvel_etat = ecran_stock_evenements(&es, &event, &ctx, &ctx.log);
                    break;
                case ETAT_RESULTAT:
                    nouvel_etat = ecran_resultat_evenements(
                        &btn_res_suite, &btn_res_retour, &event);
                    break;
                case ETAT_CAMERAS:
                    nouvel_etat = ecran_cameras_evenements(&ec, &event);
                    break;
                case ETAT_JEU:
                case ETAT_GAME_OVER:
                    nouvel_etat = ecran_jeu_evenements(&ej, &event, &ctx);
                    break;
                default:
                    break;
            }
            ctx.etat = nouvel_etat;
        }

        //MAJ
        switch (ctx.etat) {
            case ETAT_INTRO:     ecran_intro_update(&ei);       break;
            case ETAT_CAMERAS:   ecran_cameras_update(&ec);     break;
            case ETAT_JEU:
            case ETAT_GAME_OVER: ecran_jeu_update(&ej, &ctx);  break;
            default: break;
        }

        //RENDU
        switch (ctx.etat) {
            case ETAT_INTRO:
                ecran_intro_rendu(&ei, &ctx);                          break;
            case ETAT_STOCK:
                ecran_stock_rendu(&es, &ctx, &ctx.log);                break;
            case ETAT_RESULTAT:
                ecran_resultat_rendu(&ctx, &btn_res_suite,&btn_res_retour);              break;
            case ETAT_CAMERAS:
                ecran_cameras_rendu(&ec, &ctx);                        break;
            case ETAT_JEU:
            case ETAT_GAME_OVER:
                ecran_jeu_rendu(&ej, &ctx);                            break;
            default: break;
        }

        SDL_RenderPresent(ctx.renderer);

        Uint32 elapsed = SDL_GetTicks() - temps_debut;
        if (elapsed < MS_PAR_FRAME)
            SDL_Delay(MS_PAR_FRAME - elapsed);
    }

    rendu_quitter(&ctx);
    printf("[INFO] HACKNET termine proprement.\n");
    return 0;
}