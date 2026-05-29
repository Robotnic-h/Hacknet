/* ============================================================
 *  ecran_intro.c — Écran d'accueil : présentation + choix de ville
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "ecran_intro.h"
#include "rendu.h"

/* ligne afficher dan s brieff */
static const char *BRIEFING[] = {
    "> Connexion securisee etablie...",
    "> Identite masquee. Localisation: inconnue.",
    "> MISSION : vous etes le complice.",
    "> Le voleur attend vos instructions.",
    "> Infiltrez le commissariat cible.",
    "> Modifiez leur stock d'equipement.",
    "> Guidez le voleur a travers la ville.",
    "> Ne vous faites pas attraper.",
    "> Choisissez la ville a cibler. >",
    NULL
};

static const char *VILLES[3] = {"IVRY", "LAVAL", "AGEN"};

void ecran_intro_init(EcranIntro *ei) {
    memset(ei, 0, sizeof(EcranIntro));
    ei->ligne_courante  = 0;
    ei->char_courant    = 0;
    ei->timer_typo      = 0;
    ei->ville_selectee  = -1;   //pas de ville choisi

    /* mettre bouton ville */
    int btn_w = 220, btn_h = 56;
    int total_w = 3 * btn_w + 2 * 30;
    int start_x = (FENETRE_LARGEUR - total_w) / 2;
    int btn_y = 725;

    for (int i = 0; i < 3; i++) {
        ei->btn_villes[i].rect   = (SDL_Rect){
            start_x + i * (btn_w + 30), btn_y, btn_w, btn_h
        };
        strncpy(ei->btn_villes[i].label, VILLES[i], 63);
        ei->btn_villes[i].actif    = 1;
        ei->btn_villes[i].survole  = 0;
        ei->btn_villes[i].couleur  = COL_ROUGE_VIF;
    }

    /* Bouton "Infiltrer" */
    ei->btn_go.rect   = (SDL_Rect){FENETRE_LARGEUR - 260, 820, 220, 52};
    strncpy(ei->btn_go.label, "INFILTRER >", 63);
    ei->btn_go.actif   = 0;
    ei->btn_go.couleur = COL_ROUGE_VIF;
}

/* effet machine a ecrire */
static void update_typo(EcranIntro *ei) {
    if (BRIEFING[ei->ligne_courante] == NULL) return;  /* affiche tout */

    ei->timer_typo++;
    /* Un caract tt les 2 frames (~30 FPS) */
    if (ei->timer_typo < 2) return;
    ei->timer_typo = 0;

    int len = (int)strlen(BRIEFING[ei->ligne_courante]);
    if (ei->char_courant < len) {
        /* copie carct suivant dans buffer afficher */
        ei->texte_affiche[ei->ligne_courante][ei->char_courant] = BRIEFING[ei->ligne_courante][ei->char_courant];
        ei->char_courant++;
    } else {
        /* Ligne suivante a afficher */
        ei->ligne_courante++;
        ei->char_courant = 0;
    }
}

EtatJeu ecran_intro_evenements(EcranIntro *ei, SDL_Event *e, Contexte *ctx) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    /* mise a jour survole bouton */
    rendu_bouton_update(&ei->btn_go, mx, my);
    for (int i = 0; i < 3; i++)
        rendu_bouton_update(&ei->btn_villes[i], mx, my);

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        /* Clic sur une ville */
        for (int i = 0; i < 3; i++) {
            if (rendu_bouton_clique(&ei->btn_villes[i], mx, my)) {
                ei->ville_selectee = i;
                ctx->ville_choisie = i;
                /* on surligne ville choisie */
                for (int j = 0; j < 3; j++)
                    ei->btn_villes[j].survole = (j == i) ? 1 : 0;
                /* Active le bouton "Infiltrer" */
                ei->btn_go.actif = 1;
            }
        }
        //passer a l'ecran suivant si ville choisi
        if (rendu_bouton_clique(&ei->btn_go, mx, my)) {
            return ETAT_STOCK;
        }
    }
    return ETAT_INTRO;  //sinon on reste
}


void ecran_intro_update(EcranIntro *ei) {
    update_typo(ei);
}

void ecran_intro_rendu(EcranIntro *ei, Contexte *ctx) {
    SDL_Renderer *r = ctx->renderer;

    rendu_fond_grille(r, FENETRE_LARGEUR, FENETRE_HAUTEUR);

    /* titre avec effet glow */
    SDL_Color rouge_vif = COL_ROUGE_VIF;
    rendu_titre_glow(r, ctx->font_titre, "HACKNET", FENETRE_LARGEUR/2 - 200, 30, rouge_vif);

    /* Sous-titre */
    SDL_Color rouge_sombre = {120, 20, 10, 255};
    rendu_texte(r, ctx->font_petit, "// SYSTEME DE COORDINATION CRIMINELLE v2.4", 315,130, rouge_sombre);

    /* rect brief */
    SDL_Rect boite = {100, 180, FENETRE_LARGEUR - 200, 450};
    SDL_Color fond_boite = {8, 0, 0, 200};
    rendu_rect_plein(r, boite, fond_boite);
    rendu_rect_contour(r, boite, (SDL_Color){60, 0, 0, 255}, 1);

    /* nom de la zone de log */
    rendu_texte(r, ctx->font_petit, "// BRIEFING", boite.x + 10, boite.y +10 , rouge_sombre);

    /* ligne texte machine a ecrire 44px*/
    SDL_Color col_texte = {160, 40, 40, 255};
    SDL_Color col_accent = {220, 80, 40, 255};
    for (int i = 0; i <= ei->ligne_courante && BRIEFING[i] != NULL; i++) {
        SDL_Color col = (i == ei->ligne_courante) ? col_accent : col_texte;
        rendu_texte(r, ctx->font_petit, ei->texte_affiche[i], boite.x + 18, boite.y + 45 + i * 44, col);
    }

    rendu_texte_centre(r, ctx->font_petit, "// SELECTIONNER LA CIBLE", 0, FENETRE_LARGEUR, 667, rouge_sombre);

    /* bouton ville */
    for (int i = 0; i < 3; i++) {
        if (i == ei->ville_selectee)
            ei->btn_villes[i].survole = 1;
        rendu_bouton(r, ctx->font_normal, &ei->btn_villes[i]);
    }

    rendu_bouton(r, ctx->font_normal, &ei->btn_go);
}