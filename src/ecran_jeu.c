#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "ecran_jeu.h"
#include "rendu.h"
#include "astar.h"
#include "dijkstra.h"

/* zone pour affichage */
#define CARTE_X   20
#define CARTE_Y   100
#define CARTE_W   1160
#define CARTE_H   600
#define LOG_X     20
#define LOG_Y     740
#define LOG_W     940
#define LOG_H     140

/* rayon neud et player */
#define RAYON_NOEUD    10
#define RAYON_JOUEUR   13

/* vitesse deplacement entre 0 et 1 par frames */
#define VITESSE_ANIM   0.04f

static int poids_arete(Graphe *g, int a, int b) {
    for (int i = 0; i < g->nb_aretes; i++) {
        int ia = g->aretes[i].id_a;
        int ib = g->aretes[i].id_b;
        if ((ia == a && ib == b) || (!g->aretes[i].oriente && ib == a && ia == b))
            return g->aretes[i].poids;
    }
    return 0;
}

static int distance_chemin(EcranJeu *ej, Graphe *g) {
    int total = 0;
    for (int i = 0; i < ej->nb_chemin - 1; i++)
        total += poids_arete(g, ej->chemin[i], ej->chemin[i+1]);
    return total;
}

/* position perso sur neud act */
static void placer_entite(Entite *ent, Graphe *g) {
    ent->noeud_cible = ent->noeud_actuel;
    ent->progress    = 0.0f;
    ent->x = CARTE_X + g->noeuds[ent->noeud_actuel].x * CARTE_W / (float)FENETRE_LARGEUR;
    ent->y = CARTE_Y + g->noeuds[ent->noeud_actuel].y * CARTE_H / (float)FENETRE_HAUTEUR;
}

/* calcul position entre 2 neuds */
static void interpoler_entite(Entite *ent, Graphe *g) {
    if (ent->progress >= 1.0f) {
        ent->noeud_actuel = ent->noeud_cible;
        ent->progress     = 0.0f;
    }
    if (ent->progress <= 0.0f) {
        ent->x = CARTE_X + g->noeuds[ent->noeud_actuel].x * CARTE_W / (float)FENETRE_LARGEUR;
        ent->y = CARTE_Y + g->noeuds[ent->noeud_actuel].y * CARTE_H / (float)FENETRE_HAUTEUR;
        return;
    }
    float ax = CARTE_X + g->noeuds[ent->noeud_actuel].x * CARTE_W / (float)FENETRE_LARGEUR;
    float ay = CARTE_Y + g->noeuds[ent->noeud_actuel].y * CARTE_H / (float)FENETRE_HAUTEUR;
    float bx = CARTE_X + g->noeuds[ent->noeud_cible].x * CARTE_W / (float)FENETRE_LARGEUR;
    float by = CARTE_Y + g->noeuds[ent->noeud_cible].y * CARTE_H / (float)FENETRE_HAUTEUR;
    ent->x = ax + (bx - ax) * ent->progress;
    ent->y = ay + (by - ay) * ent->progress;
}

/* verif adjacence d'un noeud par rapport a l'autre */
static int est_adjacent(Graphe *g, int a, int b) {
    for (int i = 0; i < g->nb_aretes; i++) {
        int ia = g->aretes[i].id_a;
        int ib = g->aretes[i].id_b;
        if (g->aretes[i].oriente) {
            /* oriente */
            if (ia == a && ib == b) return 1;
        } else {
            /* non oriente */
            if ((ia == a && ib == b) || (ib == a && ia == b)) return 1;
        }
    }
    return 0;
}

void ecran_jeu_init(EcranJeu *ej, Contexte *ctx) {
    memset(ej, 0, sizeof(EcranJeu));

    /* Police par du commisariat */
    ctx->voleur.noeud_actuel = ctx->graphe.deb_voleur;
    ctx->police.noeud_actuel = ctx->graphe.deb_police;
    placer_entite(&ctx->voleur, &ctx->graphe);
    placer_entite(&ctx->police, &ctx->graphe);

    ctx->tour          = 0;
    ctx->jeu_en_cours  = 1;
    ej->police_en_mouvement = 0;
    ej->timer_police   = 0;
    ej->game_over      = 0;
    ej->anim_gameover  = 0;
    ej->police_bonus   = 0;

    /* Bouton rejouer */
    ej->btn_rejouer.rect   = (SDL_Rect){FENETRE_LARGEUR/2 - 110, 550, 220, 56};
    strncpy(ej->btn_rejouer.label, "REJOUER", 63);
    ej->btn_rejouer.actif   = 1;
    ej->btn_rejouer.couleur = COL_ROUGE_VIF;

    log_ajouter(&ctx->log, "> Systeme initialise.", 0);
    log_ajouter(&ctx->log, "> Voleur: noeud de depart", 0);
    log_ajouter(&ctx->log, "> Police: commissariat", 0);
    log_ajouter(&ctx->log, "> Cliquer un noeud adjacent", 1);
    log_ajouter(&ctx->log, "> pour deplacer le voleur.", 1);
}

EtatJeu ecran_jeu_evenements(EcranJeu *ej, SDL_Event *e, Contexte *ctx) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    /* si GO -> rejouer */
    if (ej->game_over) {
        rendu_bouton_update(&ej->btn_rejouer, mx, my);
        if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
            if (rendu_bouton_clique(&ej->btn_rejouer, mx, my))
                return ETAT_INTRO;
        return ETAT_GAME_OVER;
    }

    if (!ctx->jeu_en_cours) return ETAT_JEU;

    /* clic sur un noeud et voleur bouge si adja */
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        /* verif que rien n'est en cours */
        if (ctx->voleur.progress > 0.0f) return ETAT_JEU;

        Graphe *g = &ctx->graphe;
        for (int i = 0; i < g->nb_noeuds; i++) {
            int nx = CARTE_X + (int)(g->noeuds[i].x * CARTE_W / (float)FENETRE_LARGEUR);
            int ny = CARTE_Y + (int)(g->noeuds[i].y * CARTE_H / (float)FENETRE_HAUTEUR);
            /* Zone de clic autour d'un noeud */
            int dx = mx - nx, dy = my - ny;
            if (dx*dx + dy*dy <= (RAYON_NOEUD+6)*(RAYON_NOEUD+6)) {
                /* si noeud cliquer -> verif adja */
                if (est_adjacent(g, ctx->voleur.noeud_actuel, i)) {
                    /* bouge le voleur*/
                    ctx->voleur.noeud_cible = i;
                    ctx->voleur.progress    = VITESSE_ANIM;
                    ctx->tour++;

                    char msg[64];
                    snprintf(msg, 63, "> Voleur -> noeud %s", g->noeuds[i].label);
                    log_ajouter(&ctx->log, msg, 1);

                    /* police se déplace apres 2 frames */
                    ej->police_en_mouvement = 0;
                    ej->timer_police        = 20;
                } else {
                    log_ajouter(&ctx->log, "> Noeud non adjacent !", 0);
                }
                break;
            }
        }
    }
    return ETAT_JEU;
}

void ecran_jeu_update(EcranJeu *ej, Contexte *ctx) {
    if (ej->game_over) {
        if (ej->anim_gameover < 255) ej->anim_gameover += 5;
        return;
    }
    if (!ctx->jeu_en_cours) return;

    Graphe *g = &ctx->graphe;

    /* animation vol */
    if (ctx->voleur.progress > 0.0f && ctx->voleur.progress < 1.0f) {
        ctx->voleur.progress += VITESSE_ANIM;
        if (ctx->voleur.progress >= 1.0f) {
            ctx->voleur.noeud_actuel = ctx->voleur.noeud_cible;
            ctx->voleur.progress     = 0.0f;
        }
    }
    interpoler_entite(&ctx->voleur, g);

    /* ─recalcul A* a chaque frame */
    int path[MAX_NODES];
    int len = astar_graphe(g,
        ctx->police.noeud_actuel,
        ctx->voleur.noeud_actuel,
        path);

    ej->nb_chemin = (len > 0) ? len : 0;
    for (int i = 0; i < ej->nb_chemin; i++)
        ej->chemin[i] = path[i];

    /* bouge police avec un delai */
    if (ej->timer_police > 0) {
        ej->timer_police--;
        if (ej->timer_police == 0) {
            /* tirage aleatoire pour appliquer le bonus */
            ej->police_bonus = ((rand() % 100) < (int)ctx->bonus_police) ? 1 : 0;

            int prochain = (len > 1) ? path[1] : ctx->police.noeud_actuel;
            ctx->police.noeud_cible = prochain;
            ctx->police.progress    = VITESSE_ANIM;
            ej->police_en_mouvement = 1;

            if (ej->police_bonus)
                log_ajouter(&ctx->log, "> ! BONUS POLICE : +1 noeud !", 2);

            char msg[64];
            snprintf(msg, 63, "> Police -> noeud %s", g->noeuds[prochain].label);
            log_ajouter(&ctx->log, msg, 2);
        }
    }

    /* animation police */
    if (ctx->police.progress > 0.0f && ctx->police.progress < 1.0f) {
        ctx->police.progress += VITESSE_ANIM;
        if (ctx->police.progress >= 1.0f) {
            ctx->police.noeud_actuel = ctx->police.noeud_cible;
            ctx->police.progress     = 0.0f;
            ej->police_en_mouvement  = 0;
        }
    }
    interpoler_entite(&ctx->police, g);

    /* bonus police active -> avance d'un noeud de pkus */
    if (ej->police_bonus && ctx->police.progress == 0.0f
        && ej->police_en_mouvement == 0) {

        /* recalcule cehemin a chaque changement de position */
        int path2[MAX_NODES];
        int len2 = astar_graphe(g,
            ctx->police.noeud_actuel,
            ctx->voleur.noeud_actuel,
            path2);

        if (len2 > 1) {
            int prochain2 = path2[1];
            ctx->police.noeud_cible = prochain2;
            ctx->police.progress    = VITESSE_ANIM;
            ej->police_en_mouvement = 1;

            char msg[64];
            snprintf(msg, 63, "> Police bonus -> noeud %s", g->noeuds[prochain2].label);
            log_ajouter(&ctx->log, msg, 2);
        }
        ej->police_bonus = 0;  /* bonus appliquer*/
    }

    /* detect capture */
    float dx = ctx->voleur.x - ctx->police.x;
    float dy = ctx->voleur.y - ctx->police.y;
    if (sqrtf(dx*dx + dy*dy) < (float)(RAYON_JOUEUR * 2)) {
        ctx->jeu_en_cours = 0;
        ej->game_over     = 1;
        ej->anim_gameover = 0;
        log_ajouter(&ctx->log, "> !!! CAPTURE !!!", 1);
        log_ajouter(&ctx->log, "> Votre complice a ete arrete.", 1);
    }

    char msg_tour[32];
    snprintf(msg_tour, 31, "> Tour %d", ctx->tour);
    (void)msg_tour;
}

void ecran_jeu_rendu(EcranJeu *ej, Contexte *ctx) {
    SDL_Renderer *r = ctx->renderer;
    Graphe *g = &ctx->graphe;

    rendu_fond_grille(r, FENETRE_LARGEUR, FENETRE_HAUTEUR);

    rendu_titre_glow(r, ctx->font_sstitre, "POURSUITE EN COURS", FENETRE_LARGEUR/2 - 280, 20, COL_ROUGE_VIF);

    /*CARTE*/
    SDL_Rect fond_carte = {CARTE_X, CARTE_Y, CARTE_W, CARTE_H};
    rendu_rect_plein(r, fond_carte, (SDL_Color){3, 0, 0, 240});
    rendu_rect_contour(r, fond_carte, (SDL_Color){50, 0, 0, 255}, 1);

    /* aretes */
    for (int i = 0; i < g->nb_aretes; i++) {
        Noeud *a = &g->noeuds[g->aretes[i].id_a];
        Noeud *b = &g->noeuds[g->aretes[i].id_b];
        int ax = CARTE_X + (int)(a->x * CARTE_W / (float)FENETRE_LARGEUR);
        int ay = CARTE_Y + (int)(a->y * CARTE_H / (float)FENETRE_HAUTEUR);
        int bx = CARTE_X + (int)(b->x * CARTE_W / (float)FENETRE_LARGEUR);
        int by = CARTE_Y + (int)(b->y * CARTE_H / (float)FENETRE_HAUTEUR);

        /* fleche si oriente */
        if (g->aretes[i].oriente) {
            rendu_fleche(r, ax, ay, bx, by, (SDL_Color){80, 20, 20, 255}, 3);
        }
        else{
            rendu_ligne(r, ax, ay, bx, by, (SDL_Color){80, 20, 20, 255}, 3);
        }
    }

    /* parcour police en surbrillance */
    for (int i = 0; i < ej->nb_chemin - 1; i++) {
        int na = ej->chemin[i];
        int nb = ej->chemin[i + 1];
        int ax = CARTE_X + (int)(g->noeuds[na].x * CARTE_W / (float)FENETRE_LARGEUR);
        int ay = CARTE_Y + (int)(g->noeuds[na].y * CARTE_H / (float)FENETRE_HAUTEUR);
        int bx = CARTE_X + (int)(g->noeuds[nb].x * CARTE_W / (float)FENETRE_LARGEUR);
        int by = CARTE_Y + (int)(g->noeuds[nb].y * CARTE_H / (float)FENETRE_HAUTEUR);
        rendu_ligne(r, ax, ay, bx, by, (SDL_Color){50, 100, 220, 200}, 5);
    }

    /* Noeud */
    int voisin_actuel = ctx->voleur.noeud_actuel;
    for (int i = 0; i < g->nb_noeuds; i++) {
        int nx = CARTE_X + (int)(g->noeuds[i].x * CARTE_W / (float)FENETRE_LARGEUR);
        int ny = CARTE_Y + (int)(g->noeuds[i].y * CARTE_H / (float)FENETRE_HAUTEUR);

        /* effet si adja a celui du vol */
        int est_voisin = est_adjacent(g, voisin_actuel, i);
        if (est_voisin) {
            rendu_cercle(r, nx, ny, RAYON_NOEUD + 8, (SDL_Color){80, 0, 0, 60});
            rendu_cercle_contour(r, nx, ny, RAYON_NOEUD + 8,
                (SDL_Color){120, 0, 0, 150});
        }

        /* le noeud */
        SDL_Color col_noeud = (i == ctx->police.noeud_actuel)
            ? (SDL_Color){30, 50, 120, 255}   /* bleu si police dessus */
            : (SDL_Color){40,  0,   0, 255};  // classique sinon
        rendu_cercle(r, nx, ny, RAYON_NOEUD, col_noeud);
        rendu_cercle_contour(r, nx, ny, RAYON_NOEUD, est_voisin ? (SDL_Color){150, 0, 0, 255} : (SDL_Color){ 80, 0, 0, 255});

        /* nom du neud */
        rendu_texte(r, ctx->font_petit, g->noeuds[i].label, nx + 12, ny - 8, (SDL_Color){80, 0, 0, 200});
    }

    /* police */
    int px = (int)ctx->police.x, py = (int)ctx->police.y;
    rendu_cercle(r, px, py, RAYON_JOUEUR + 6, (SDL_Color){50, 100, 200, 40});
    rendu_cercle(r, px, py, RAYON_JOUEUR, (SDL_Color){50, 100, 220, 255});
    rendu_cercle_contour(r, px, py, RAYON_JOUEUR, (SDL_Color){100, 150, 255, 255});
    /* nom P */
    rendu_texte(r, ctx->font_petit, "P", px - 4, py - 6, (SDL_Color){200, 230, 255, 255});

    /* voleur ── */
    int vx = (int)ctx->voleur.x, vy = (int)ctx->voleur.y;
    rendu_cercle(r, vx, vy, RAYON_JOUEUR + 6, (SDL_Color){200, 50, 50, 40});
    rendu_cercle(r, vx, vy, RAYON_JOUEUR, (SDL_Color){200, 40, 40, 255});
    rendu_cercle_contour(r, vx, vy, RAYON_JOUEUR, (SDL_Color){255, 80, 80, 255});
    /* nom vol */
    rendu_texte(r, ctx->font_petit, "V",
        vx - 4, vy - 6, (SDL_Color){255, 200, 200, 255});

    /* zone log */
    rendu_zone_log(r, ctx->font_petit, &ctx->log,
        LOG_X , LOG_Y, LOG_W, LOG_H);

    /*info tour */
    SDL_Rect fond_info = {LOG_X + LOG_W +20, LOG_Y, 200, LOG_H};
    rendu_rect_plein(r, fond_info, (SDL_Color){8, 0, 0, 220});
    rendu_rect_contour(r, fond_info, (SDL_Color){80, 0, 0, 255}, 1);

    char txt[32];
    snprintf(txt, 31, "TOUR : %d", ctx->tour);
    rendu_texte(r, ctx->font_petit, txt,LOG_X + LOG_W +25, LOG_Y+3, (SDL_Color){160, 40, 40, 255});

    int dd = distance_chemin(ej, g);
    snprintf(txt, 31, "DIST : %d min", dd);
    rendu_texte(r, ctx->font_petit, txt, LOG_X + LOG_W +25, LOG_Y+26, (SDL_Color){120, 20, 20, 255});

    /* LEGENDE */
    rendu_cercle(r, LOG_X + LOG_W +35, LOG_Y+92, 8,(SDL_Color){200, 40, 40, 255});
    rendu_texte(r, ctx->font_petit, "Voleur (vous)", LOG_X + LOG_W +45, LOG_Y+79, (SDL_Color){160, 40, 40, 255});

    rendu_cercle(r, LOG_X + LOG_W +35, LOG_Y+114, 8,(SDL_Color){50, 100, 220, 255});
    rendu_texte(r, ctx->font_petit, "Police (IA)",LOG_X + LOG_W +45, LOG_Y+102, (SDL_Color){80, 120, 200, 255});

    /* fenetre GO */
    if (ej->game_over) {
        dessiner_game_over(ej, ctx);
    }
}

/* fenetre GO */
void dessiner_game_over(EcranJeu *ej, Contexte *ctx) {
    SDL_Renderer *r = ctx->renderer;

    /* Assombrit ecran */
    int alpha = ej->anim_gameover > 180 ? 180 : ej->anim_gameover;
    SDL_SetRenderDrawColor(r, 0, 0, 0, (Uint8)alpha);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_Rect overlay = {0, 0, FENETRE_LARGEUR, FENETRE_HAUTEUR};
    SDL_RenderFillRect(r, &overlay);

    int bx = FENETRE_LARGEUR/2 - 280, by = FENETRE_HAUTEUR/2 - 160;
    int bw = 560, bh = 320;

    SDL_Rect boite = {bx, by, bw, bh};
    rendu_rect_plein(r, boite, (SDL_Color){10, 0, 0, 240});

    rendu_rect_contour(r, boite, (SDL_Color){200, 0, 0, 255}, 3);
    SDL_Rect boite2 = {bx - 4, by - 4, bw + 8, bh + 8};
    rendu_rect_contour(r, boite2, (SDL_Color){100, 0, 0, 150}, 2);

    //Titre
    rendu_texte_centre(r, ctx->font_sstitre, "!!! ALERTE !!!",bx, bw, by + 18, (SDL_Color){255, 0, 0, 255});

    rendu_ligne(r, bx + 20, by + 72, bx + bw - 20, by + 72,(SDL_Color){100, 0, 0, 255}, 1);

    /* MESSAGE */
    rendu_texte_centre(r, ctx->font_normal,"VOTRE COMPLICE A ETE ATTRAPE",bx, bw, by + 90, (SDL_Color){220, 50, 50, 255});

    rendu_texte_centre(r, ctx->font_normal,"VOUS AVEZ ETE REPERE",bx, bw, by + 120, (SDL_Color){200, 40, 40, 255});

    rendu_ligne(r, bx + 20, by + 152, bx + bw - 20, by + 152,(SDL_Color){80, 0, 0, 255}, 1);

    /*Urgence*/
    rendu_texte_centre(r, ctx->font_petit,"LA POLICE ARRIVE.",bx, bw, by + 164, (SDL_Color){180, 30, 30, 255});

    rendu_texte_centre(r, ctx->font_petit,"COUPEZ LA CONNEXION IMMEDIATEMENT.",bx, bw, by + 186, (SDL_Color){150, 20, 20, 255});

    rendu_texte_centre(r, ctx->font_petit,"NE LAISSEZ AUCUNE TRACE.",bx, bw, by + 208, (SDL_Color){120, 10, 10, 255});

    /* code errrreur */
    char code[64];
    snprintf(code, 63, "ERR_CODE: 0x%04X — TOUR %d",(ctx->tour * 137 + 0xDEAD) & 0xFFFF, ctx->tour);
    rendu_texte_centre(r, ctx->font_petit, code,bx, bw, by + 238, (SDL_Color){120, 10, 10, 255});

    rendu_bouton(r, ctx->font_normal, &ej->btn_rejouer);
}