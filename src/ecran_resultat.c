#include <stdio.h>
#include <string.h>
#include "ecran_resultat.h"
#include "rendu.h"



// resultat stock pris par la poilce
static int quantites_embarquees[NB_OBJETS];

// init
void ecran_resultat_init(EcranResultat *er, Stock *stock,Bouton *btn_suite, Bouton *btn_retour) {
    memset(er, 0, sizeof(EcranResultat));

    //Algo knapsack
typedef struct
 {
    int valeur;
    int poids;
    int index_objet;
} Item;

// Tableau temporaire avec TOUS les objets disponibles 
Item items[256];
int nb_items = 0;

for (int i = 0; i < NB_OBJETS; i++) 
{
    quantites_embarquees[i] = 0;
}

//Chaque item deviens un objets
for (int i = 0; i < stock->nb_objets; i++)
 {
    for (int q = 0; q < stock->objets[i].quantite; q++)
     {
        items[nb_items].valeur =(int)(stock->objets[i].bonus * 100);
        items[nb_items].poids =stock->objets[i].poids;
        items[nb_items].index_objet = i;
        nb_items++;
    }
}


int W = POIDS_MAX_VOITURE;
int dp[nb_items + 1][W + 1];
// iNIt
for (int i = 0; i <= nb_items; i++) {
    for (int w = 0; w <= W; w++) {
        dp[i][w] = 0;
    }
}

for (int i = 1; i <= nb_items; i++) 
{
    int val = items[i - 1].valeur;
    int wt  = items[i - 1].poids;
    for (int w = 0; w <= W; w++) {
        if (wt <= w) {
            int sans_objet = dp[i - 1][w];
            int avec_objet = dp[i - 1][w - wt] + val;
            dp[i][w] =
                (sans_objet > avec_objet)
                ? sans_objet
                : avec_objet;
        } else {
            dp[i][w] = dp[i - 1][w];
        }
    }
}

//traceback
for (int i = nb_items; i > 0 && W > 0; i--) 
{

    if (dp[i][W] != dp[i - 1][W]) {
        int idx = items[i - 1].index_objet;
        quantites_embarquees[idx]++;
        W -= items[i - 1].poids;
    }
}
    btn_suite->rect   = (SDL_Rect){FENETRE_LARGEUR - 260, 820, 220, 52};
    strncpy(btn_suite->label, "CAMERAS >", 63);
    btn_suite->actif   = 1;
    btn_suite->couleur = COL_ROUGE_VIF;

    btn_retour->rect   = (SDL_Rect){40, 820, 180, 52};
    strncpy(btn_retour->label, "< RETOUR", 63);
    btn_retour->actif   = 1;
    btn_retour->couleur = COL_ROUGE_SOMBRE;
}

EtatJeu ecran_resultat_evenements(Bouton *btn_suite, Bouton *btn_retour,SDL_Event *e) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    rendu_bouton_update(btn_suite,  mx, my);
    rendu_bouton_update(btn_retour, mx, my);
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        if (rendu_bouton_clique(btn_retour, mx, my)) return ETAT_STOCK;
        if (rendu_bouton_clique(btn_suite,  mx, my)) return ETAT_CAMERAS;
    }
    return ETAT_RESULTAT;
}

void ecran_resultat_rendu(Contexte *ctx, Bouton *btn_suite, Bouton *btn_retour) {
    SDL_Renderer *r = ctx->renderer;
    Stock *stock = &ctx->stock;

    rendu_fond_grille(r, FENETRE_LARGEUR, FENETRE_HAUTEUR);
    rendu_titre_glow(r, ctx->font_sstitre, "CHARGEMENT VOITURE DE POLICE",FENETRE_LARGEUR/2 - 450, 50, COL_ROUGE_VIF);
    rendu_texte(r, ctx->font_petit, "// UNE VOITURE A ETE ENVOYER SUITE A L'ALERTE DECLENCHE PAR VOTRE COMPLICE",120, 137, (SDL_Color){120, 20, 10, 255});
    rendu_texte(r, ctx->font_petit, "// Voici l'équipement embarqué par la police dans leur voiture",200, 177,(SDL_Color){120, 20, 10, 255});

    // Barre de poids charger
    int poids_charge = 0;
    float bonus_charge = 0;
    for (int i = 0; i < stock->nb_objets; i++) {
        poids_charge += quantites_embarquees[i] * stock->objets[i].poids;
        bonus_charge += quantites_embarquees[i] * stock->objets[i].bonus;
    }
    ctx->bonus_police = bonus_charge; 

    char txt_barre[64];
    snprintf(txt_barre, 63, "POIDS CHARGE : %d / %d kg  |  BONUS TOTAL POLICE : %.1f %%",poids_charge, POIDS_MAX_VOITURE, bonus_charge);
    rendu_texte(r, ctx->font_petit, txt_barre,224, 287, (SDL_Color){255, 80, 0, 255});

    // Grille des resultat
    int cols = 3;
    int card_w = 300, card_h = 200;
    int gap    = 12;
    int total_w = cols * card_w + (cols-1) * gap;
    int start_x = (FENETRE_LARGEUR - total_w) / 2;
    int start_y = 330;

    for (int i = 0; i < stock->nb_objets; i++) {
        int col = i % cols;
        int row = i / cols;
        int cx  = start_x + col * (card_w + gap);
        int cy  = start_y + row * (card_h + gap);
        ObjetStock *obj = &stock->objets[i];

        SDL_Rect carte = {cx, cy, card_w, card_h};
        rendu_rect_plein(r, carte, (SDL_Color){10, 0, 0, 200});
        rendu_rect_contour(r, carte, (SDL_Color){60, 0, 0, 255}, 1);

        /* Nom */
        rendu_texte_centre(r, ctx->font_petit, obj->nom,
            cx, card_w, cy + 25, (SDL_Color){150, 30, 30, 255});

        // Quantite embarquer
        char qty[8];
        snprintf(qty, 7, "%d", quantites_embarquees[i]);
        rendu_texte_centre(r, ctx->font_titre, qty,cx, card_w, cy + 52, (SDL_Color){200, 50, 50, 255});

        rendu_texte_centre(r, ctx->font_petit, "embarques",cx, card_w, cy + 170, (SDL_Color){120, 20, 10, 255});
    }

    rendu_bouton(r, ctx->font_normal, btn_retour);
    rendu_bouton(r, ctx->font_normal, btn_suite);
}