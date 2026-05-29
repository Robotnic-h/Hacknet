#include <stdio.h>
#include <string.h>
#include "ecran_stock.h"
#include "rendu.h"

static const ObjetStock STOCK_DEFAUT[NB_OBJETS] = {
    {"GYROPHARE", 2,  5,  2},
    {"MENOTTES",  8,   2, 0.5},
    {"TALKIE WALKIE", 5,  8,  4},
    {"SIFFLET", 8,   1, 0.7},
    {"PISTOLET",8,  10,  5},
    {"BROUILLEUR", 5,   18, 9},
};

//init stock avec valeur par def
void ecran_stock_init(EcranStock *es, Stock *stock) {
    memset(es, 0, sizeof(EcranStock));

    stock->nb_objets = NB_OBJETS;
    for (int i = 0; i < NB_OBJETS; i++)
        stock->objets[i] = STOCK_DEFAUT[i];

    /* Bouton Valider */
    es->btn_valider.rect   = (SDL_Rect){FENETRE_LARGEUR - 260, 820, 220, 52};
    strncpy(es->btn_valider.label, "VALIDER >", 63);
    es->btn_valider.actif   = 1;
    es->btn_valider.couleur = COL_ROUGE_VIF;

    /* Bouton Retour */
    es->btn_retour.rect   = (SDL_Rect){40, 820, 180, 52};
    strncpy(es->btn_retour.label, "< RETOUR", 63);
    es->btn_retour.actif   = 1;
    es->btn_retour.couleur = COL_ROUGE_SOMBRE;

    /* 2 col et 3 l sur 1200x800 */
    int col_x[2] = {60, 640};
    int start_y  = 300;
    int item_h   = 160;
    int item_w   = 520;

    for (int i = 0; i < NB_OBJETS; i++) {
        int col  = i / 3;
        int row  = i % 3;
        int base_x = col_x[col];
        int base_y = start_y + row * item_h;

        /* Bouton + */
        es->btn_plus[i].rect   = (SDL_Rect){base_x + item_w - 100, base_y + 20, 44, 44};
        strncpy(es->btn_plus[i].label, "+", 63);
        es->btn_plus[i].actif   = 1;
        es->btn_plus[i].couleur = COL_ROUGE_VIF;

        /* Bouton - */
        es->btn_moins[i].rect   = (SDL_Rect){base_x + item_w - 50, base_y + 20, 44, 44};
        strncpy(es->btn_moins[i].label, "-", 63);
        es->btn_moins[i].actif   = 1;
        es->btn_moins[i].couleur = COL_ROUGE_VIF;
    }
}

//cal poids tot et bonus tot possible
static void calcul_totaux(Stock *stock, int *poids_total, float *bonus_total) {
    *poids_total = 0;
    *bonus_total = 0;
    for (int i = 0; i < stock->nb_objets; i++) {
        *poids_total += stock->objets[i].quantite * stock->objets[i].poids;
        *bonus_total += stock->objets[i].quantite * stock->objets[i].bonus;
    }
}

/* gestion even */
EtatJeu ecran_stock_evenements(EcranStock *es, SDL_Event *e,Contexte *ctx, ZoneLog *log) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    /* MAJ survol */
    rendu_bouton_update(&es->btn_valider, mx, my);
    rendu_bouton_update(&es->btn_retour, mx, my);
    for (int i = 0; i < NB_OBJETS; i++) {
        rendu_bouton_update(&es->btn_plus[i],  mx, my);
        rendu_bouton_update(&es->btn_moins[i], mx, my);
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        /* Bouton retour */
        if (rendu_bouton_clique(&es->btn_retour, mx, my))
            return ETAT_INTRO;

        /* Bouton valider pour aller a ecran suivant*/
        if (rendu_bouton_clique(&es->btn_valider, mx, my)) {
            int p;
            float b;
            calcul_totaux(&ctx->stock, &p, &b);

            /* poids max 60 kg */
            if (p < 60) {
                log_ajouter(log, "> ERREUR : poids minimum 60 kg requis !", 1);
                return ETAT_STOCK;   //ne vhange pas
            }

            ctx->poids_voiture = p;
            ctx->bonus_police  = b;
            char msg[128];
            snprintf(msg, 127, "> Stock valide. Poids: %d kg | Bonus: %.1f %%", p, b);
            log_ajouter(log, msg, 1);
            return ETAT_RESULTAT;
        }

        /* Boutons + / - */
        for (int i = 0; i < NB_OBJETS; i++) {
            ObjetStock *obj = &ctx->stock.objets[i];
            char msg[128];

            if (rendu_bouton_clique(&es->btn_plus[i], mx, my)) {
                obj->quantite++;
                snprintf(msg, 127, "> %s: %d unite(s) [+1]", obj->nom, obj->quantite);
                log_ajouter(log, msg, 1);
            }
            if (rendu_bouton_clique(&es->btn_moins[i], mx, my) && obj->quantite > 0) {
                obj->quantite--;
                snprintf(msg, 127, "> %s: %d unite(s) [-1]", obj->nom, obj->quantite);
                log_ajouter(log, msg, 1);
            
            }
        }
    }
    return ETAT_STOCK;
}

void ecran_stock_rendu(EcranStock *es, Contexte *ctx, ZoneLog *log) {
    SDL_Renderer *r = ctx->renderer;
    Stock *stock = &ctx->stock;

    rendu_fond_grille(r, FENETRE_LARGEUR, FENETRE_HAUTEUR);

    /* Titre */
    rendu_titre_glow(r, ctx->font_sstitre, "STOCK COMMISSARIAT", FENETRE_LARGEUR/2 - 295, 20, COL_ROUGE_VIF);
    SDL_Color rouge_sombre = {120, 20, 10, 255};
    rendu_texte(r, ctx->font_petit, "// OBJECTIF - TRAFIQUER LES STOCKS DU COMMISARIAT - MIN 60 kg",210, 85, rouge_sombre);

    /* Barre poids */
    int poids_total;
    float bonus_total;
    calcul_totaux(stock, &poids_total, &bonus_total);

    SDL_Rect barre_fond = {60, 170, FENETRE_LARGEUR - 120, 24};
    rendu_rect_plein(r, barre_fond, (SDL_Color){20, 0, 0, 255});
    rendu_rect_contour(r, barre_fond, (SDL_Color){60, 0, 0, 255}, 1);

    float ratio = (float)poids_total / POIDS_MAX_VOITURE;
    if (ratio > 1.0f) ratio = 1.0f;
    SDL_Color col_barre = (poids_total > POIDS_MAX_VOITURE) ? (SDL_Color){255, 50, 0, 255} : (SDL_Color){180, 0, 0, 255};
    SDL_Rect barre_fill = {barre_fond.x+1, barre_fond.y+1,(int)((barre_fond.w-2)*ratio), barre_fond.h-2};
    rendu_rect_plein(r, barre_fill, col_barre);

    char txt_poids[64];
    snprintf(txt_poids, 63, "POIDS : %d kg |  BONUS MAX POLICE : %.1f %% de vitesse",poids_total, bonus_total);
    SDL_Color col_poids = {255, 80, 0, 255};
    rendu_texte(r, ctx->font_petit, txt_poids, 240, 130, col_poids);

    /* grille colonne ligne affichage */
    int col_x[2] = {60, 640};
    int start_y  = 220;
    int item_h   = 160;
    int item_w   = 520;

    for (int i = 0; i < NB_OBJETS; i++) {
        int col    = i / 3;
        int row    = i % 3;
        int base_x = col_x[col];
        int base_y = start_y + row * item_h;

        ObjetStock *obj = &stock->objets[i];

        /* Fond de la carte */
        SDL_Rect carte = {base_x, base_y, item_w, item_h - 10};
        rendu_rect_plein(r, carte, (SDL_Color){10, 0, 0, 200});
        rendu_rect_contour(r, carte, (SDL_Color){50, 0, 0, 255}, 1);

        /* Nom */
        rendu_texte(r, ctx->font_normal, obj->nom,base_x + 20, base_y + 14, (SDL_Color){180, 40, 40, 255});

        /* Poids et bonus */
        char infos[64];
        snprintf(infos, 63, "Poids: %d kg", obj->poids);
        rendu_texte(r, ctx->font_petit, infos,base_x + 40, base_y + 60, (SDL_Color){80, 0, 0, 255});

        snprintf(infos, 63, "Bonus Police: +%.1f %%", obj->bonus);
        rendu_texte(r, ctx->font_petit, infos,base_x + 40, base_y + 100, (SDL_Color){80, 0, 0, 255});

        /* Quantite */
        char qty[8];
        snprintf(qty, 7, "%d", obj->quantite);
        
        if( obj->quantite > 9){
            rendu_texte(r, ctx->font_sstitre, qty, base_x + item_w - 85, base_y + 20, (SDL_Color){200, 50, 50, 255});

        }
        else{
            rendu_texte(r, ctx->font_sstitre, qty, base_x + item_w - 70, base_y + 20, (SDL_Color){200, 50, 50, 255});
        }

        /* Boutons + et - */
        rendu_bouton(r, ctx->font_normal, &es->btn_plus[i]);
        rendu_bouton(r, ctx->font_normal, &es->btn_moins[i]);
    }

    /* LOG */
    rendu_zone_log(r, ctx->font_petit, log,60, 733, FENETRE_LARGEUR - 120, 66);

    /* bouton nav */
    rendu_bouton(r, ctx->font_normal, &es->btn_retour);
    rendu_bouton(r, ctx->font_normal, &es->btn_valider);
}