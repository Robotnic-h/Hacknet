#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ecran_cameras.h"
#include "rendu.h"
#include "ecran_intro.h"
#include "kruskal.h"

void ecran_cameras_init(EcranCameras *ec, Graphe *graphe) {
    memset(ec, 0, sizeof(EcranCameras));

    // Tableau cover initialisé à 0
    int cover[MAX_NOEUDS];
    memset(cover, 0, sizeof(cover));

    // Appel correct : aretes, nb_aretes, nb_noeuds, cover
    int nb = kruskal_vertex_cover(
        graphe->aretes,
        graphe->nb_aretes,
        graphe->nb_noeuds,
        cover
    );

    // Applique le résultat sur les noeuds
    for (int i = 0; i < graphe->nb_noeuds; i++)
        graphe->noeuds[i].a_camera = cover[i];

    // Compte les caméras (= valeur de retour, mais on re-compte pour cohérence)
    ec->nb_cameras = nb;

    ec->btn_suite.rect   = (SDL_Rect){FENETRE_LARGEUR - 260, 820, 220, 52};
    strncpy(ec->btn_suite.label, "LANCER >", 63);
    ec->btn_suite.actif   = 1;
    ec->btn_suite.couleur = COL_ROUGE_VIF;

    ec->btn_retour.rect   = (SDL_Rect){40, 820, 180, 52};
    strncpy(ec->btn_retour.label, "< RETOUR", 63);
    ec->btn_retour.actif   = 1;
    ec->btn_retour.couleur = COL_ROUGE_SOMBRE;

    ec->timer_pulse = 0;
}

EtatJeu ecran_cameras_evenements(EcranCameras *ec, SDL_Event *e) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    rendu_bouton_update(&ec->btn_suite,  mx, my);
    rendu_bouton_update(&ec->btn_retour, mx, my);
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        if (rendu_bouton_clique(&ec->btn_retour, mx, my)) return ETAT_RESULTAT;
        if (rendu_bouton_clique(&ec->btn_suite,  mx, my)) return ETAT_JEU;
    }
    return ETAT_CAMERAS;
}

void ecran_cameras_update(EcranCameras *ec) {
    ec->timer_pulse = (ec->timer_pulse + 1) % 120;
}

void ecran_cameras_rendu(EcranCameras *ec, Contexte *ctx) {
    SDL_Renderer *r = ctx->renderer;
    Graphe *g = &ctx->graphe;

    int carte_x = 20, carte_y = 100;
    int carte_w = 1160, carte_h = 600;

    rendu_fond_grille(r, FENETRE_LARGEUR, FENETRE_HAUTEUR);
    rendu_titre_glow(r, ctx->font_sstitre, "SURVEILLANCE — CAMERAS",
        FENETRE_LARGEUR/2 - 350, 20, COL_ROUGE_VIF);

    SDL_Rect fond_carte = {carte_x, carte_y, carte_w, carte_h};
    rendu_rect_plein(r, fond_carte, (SDL_Color){5, 0, 0, 230});
    rendu_rect_contour(r, fond_carte, (SDL_Color){50, 0, 0, 255}, 1);

    /* dessine arrete */
    for (int i = 0; i < g->nb_aretes; i++) {
        Noeud *a = &g->noeuds[g->aretes[i].id_a];
        Noeud *b = &g->noeuds[g->aretes[i].id_b];
        int ax = carte_x + (int)(a->x * carte_w / (float)FENETRE_LARGEUR);
        int ay = carte_y + (int)(a->y * carte_h / (float)FENETRE_HAUTEUR);
        int bx = carte_x + (int)(b->x * carte_w / (float)FENETRE_LARGEUR);
        int by = carte_y + (int)(b->y * carte_h / (float)FENETRE_HAUTEUR);
        rendu_ligne(r, ax, ay, bx, by, (SDL_Color){80, 20, 20, 255}, 2);
    }

    /* dessine noeud */
    for (int i = 0; i < g->nb_noeuds; i++) {
        Noeud *n = &g->noeuds[i];
        int nx = carte_x + (int)(n->x * carte_w / (float)FENETRE_LARGEUR);
        int ny = carte_y + (int)(n->y * carte_h / (float)FENETRE_HAUTEUR);

        SDL_Color col_noeud = n->a_camera
            ? (SDL_Color){200, 160,   0, 255}
            : (SDL_Color){ 80,   0,   0, 255};

        rendu_cercle(r, nx, ny, 10, col_noeud);
        rendu_cercle_contour(r, nx, ny, 10,
            n->a_camera
                ? (SDL_Color){255, 220,  50, 255}
                : (SDL_Color){120,   0,   0, 255});

        rendu_texte(r, ctx->font_petit, n->label,
            nx + 12, ny - 8,
            (SDL_Color){100, 0, 0, 255});
    }

    /* panneau infos */
    int info_x = 250, info_y = 720, info_w = 310;

    SDL_Rect fond_info = {info_x, info_y, info_w, 150};
    rendu_rect_plein(r, fond_info, (SDL_Color){8, 0, 0, 220});
    rendu_rect_contour(r, fond_info, (SDL_Color){50, 0, 0, 255}, 1);

    rendu_texte(r, ctx->font_petit, "// STATISTIQUES",
        info_x + 8, info_y + 7, (SDL_Color){120, 20, 10, 255});

    char txt[64];
    snprintf(txt, 63, "Cameras : %d", ec->nb_cameras);
    rendu_texte(r, ctx->font_petit, txt,
        info_x + 8, info_y + 50, (SDL_Color){160, 130, 0, 255});

    snprintf(txt, 63, "Noeuds : %d", g->nb_noeuds);
    rendu_texte(r, ctx->font_petit, txt,
        info_x + 8, info_y + 85, (SDL_Color){160, 40, 40, 255});

    snprintf(txt, 63, "Noeuds sans cam : %d", g->nb_noeuds - ec->nb_cameras);
    rendu_texte(r, ctx->font_petit, txt,
        info_x + 8, info_y + 120, (SDL_Color){120, 20, 20, 255});

    /* Legende */
    SDL_Rect fond_leg = {info_x + info_w + 20, info_y, info_w, 150};
    rendu_rect_plein(r, fond_leg, (SDL_Color){8, 0, 0, 220});
    rendu_rect_contour(r, fond_leg, (SDL_Color){50, 0, 0, 255}, 1);

    rendu_texte(r, ctx->font_petit, "// LEGENDE",
        info_x + info_w + 28, info_y + 7, (SDL_Color){120, 20, 10, 255});

    rendu_cercle(r, info_x + info_w + 40, info_y + 65, 8, (SDL_Color){200, 160, 0, 255});
    rendu_texte(r, ctx->font_petit, "Noeud surveille",
        info_x + info_w + 55, info_y + 50, (SDL_Color){160, 130, 0, 255});

    rendu_cercle(r, info_x + info_w + 40, info_y + 115, 8, (SDL_Color){80, 0, 0, 255});
    rendu_texte(r, ctx->font_petit, "Angle mort",
        info_x + info_w + 55, info_y + 100, (SDL_Color){120, 20, 20, 255});

    rendu_bouton(r, ctx->font_normal, &ec->btn_retour);
    rendu_bouton(r, ctx->font_normal, &ec->btn_suite);
}