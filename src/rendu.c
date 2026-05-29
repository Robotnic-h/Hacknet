#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rendu.h"

//init de la fenetre 
int rendu_init(Contexte *ctx) {
    //Init video SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[ERREUR] SDL_Init : %s\n", SDL_GetError());
        return 0;
    }

    //init SDL2_ttf pour texte
    if (TTF_Init() != 0) {
        fprintf(stderr, "[ERREUR] TTF_Init : %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    //creer fenetre principale
    ctx->fenetre = SDL_CreateWindow(
        "HACKNET - Systeme de coordination",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FENETRE_LARGEUR, FENETRE_HAUTEUR,
        SDL_WINDOW_SHOWN
    );
    if (!ctx->fenetre) {
        fprintf(stderr, "[ERREUR] CreateWindow : %s\n", SDL_GetError());
        TTF_Quit(); SDL_Quit();
        return 0;
    }

    //creer le renderer de SDL2
    ctx->renderer = SDL_CreateRenderer(
        ctx->fenetre, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!ctx->renderer) {
        fprintf(stderr, "[ERREUR] CreateRenderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(ctx->fenetre);
        TTF_Quit(); SDL_Quit();
        return 0;
    }

    //applique la police qui est dans asset
    ctx->font_titre  = TTF_OpenFont("assets/font.ttf", 100);
    ctx->font_sstitre  = TTF_OpenFont("assets/font.ttf", 60);
    ctx->font_normal = TTF_OpenFont("assets/font.ttf", 32);
    ctx->font_petit  = TTF_OpenFont("assets/font.ttf", 24);

    //si erreur
    if (!ctx->font_titre || !ctx->font_normal || !ctx->font_petit) {
        fprintf(stderr, "[AVERT] Police non trouvée, utilisation du fallback\n");
    }

    return 1;
}

//libere une fois fini
void rendu_quitter(Contexte *ctx) {
    if (ctx->font_petit)  TTF_CloseFont(ctx->font_petit);
    if (ctx->font_normal) TTF_CloseFont(ctx->font_normal);
    if (ctx->font_titre)  TTF_CloseFont(ctx->font_titre);
    if (ctx->renderer)    SDL_DestroyRenderer(ctx->renderer);
    if (ctx->fenetre)     SDL_DestroyWindow(ctx->fenetre);
    TTF_Quit();
    SDL_Quit();
}

//Rect rempli
void rendu_rect_plein(SDL_Renderer *r, SDL_Rect rect, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &rect);
}

//Contour rect
void rendu_rect_contour(SDL_Renderer *r, SDL_Rect rect, SDL_Color c, int epaisseur) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    for (int i = 0; i < epaisseur; i++) {
        SDL_Rect inner = {rect.x + i, rect.y + i,
                          rect.w - 2*i, rect.h - 2*i};
        SDL_RenderDrawRect(r, &inner);
    }
}

//cercle pleins
void rendu_cercle(SDL_Renderer *r, int cx, int cy, int rayon, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    /* Remplit le cercle en traçant des lignes horizontales */
    for (int dy = -rayon; dy <= rayon; dy++) {
        int dx = (int)sqrt((double)(rayon*rayon - dy*dy));
        SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

/* contour cercle */
void rendu_cercle_contour(SDL_Renderer *r, int cx, int cy, int rayon, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    int x = rayon, y = 0;
    int err = 0;
    while (x >= y) {
        /* Trace les 8 octants du cercle */
        SDL_RenderDrawPoint(r, cx + x, cy + y);
        SDL_RenderDrawPoint(r, cx + y, cy + x);
        SDL_RenderDrawPoint(r, cx - y, cy + x);
        SDL_RenderDrawPoint(r, cx - x, cy + y);
        SDL_RenderDrawPoint(r, cx - x, cy - y);
        SDL_RenderDrawPoint(r, cx - y, cy - x);
        SDL_RenderDrawPoint(r, cx + y, cy - x);
        SDL_RenderDrawPoint(r, cx + x, cy - y);
        y++;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) { x--; err += 1 - 2*x; }
    }
}

//ligne epaisse
void rendu_ligne(SDL_Renderer *r, int x1, int y1, int x2, int y2, SDL_Color c, int epaisseur) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    float len = sqrtf(dx*dx + dy*dy);
    if (len == 0) return;
    float nx = -dy / len;  /* normale unitaire */
    float ny =  dx / len;
    for (int i = -(epaisseur/2); i <= epaisseur/2; i++) {
        SDL_RenderDrawLine(r,
            (int)(x1 + nx*i), (int)(y1 + ny*i),
            (int)(x2 + nx*i), (int)(y2 + ny*i));
    }
}

/*Dessiner fleche*/
void rendu_fleche(SDL_Renderer *r, int x1, int y1, int x2, int y2, SDL_Color c, int epaisseur){

    rendu_ligne(r, x1, y1, x2, y2, c, epaisseur);

    double angle = atan2((double)(y2 - y1),(double)(x2 - x1));

    const double taille_fleche = 25.0;

    double angle1 = angle + M_PI / 6.0;
    double angle2 = angle - M_PI / 6.0;

    int x3 = x2 - (int)(taille_fleche * cos(angle1));
    int y3 = y2 - (int)(taille_fleche * sin(angle1));

    int x4 = x2 - (int)(taille_fleche * cos(angle2));
    int y4 = y2 - (int)(taille_fleche * sin(angle2));

    rendu_ligne(r, x2, y2, x3, y3, c, epaisseur);
    rendu_ligne(r, x2, y2, x4, y4, c, epaisseur);

}

// affichage texte a une position precise
void rendu_texte(SDL_Renderer *r, TTF_Font *f, const char *txt,
                 int x, int y, SDL_Color c) {
    if (!f || !txt) return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(f, txt, c);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    if (tex) {
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(r, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

//centre texte
void rendu_texte_centre(SDL_Renderer *r, TTF_Font *f, const char *txt,
                        int zone_x, int zone_largeur, int y, SDL_Color c) {
    if (!f || !txt) return;
    int w, h;
    TTF_SizeUTF8(f, txt, &w, &h);
    int x = zone_x + (zone_largeur - w) / 2;
    rendu_texte(r, f, txt, x, y-10, c);
}

//dessin bouton
void rendu_bouton(SDL_Renderer *r, TTF_Font *f, Bouton *b) {
    if (!b->actif) {
        /* bouton desactive*/
        SDL_Color fond_off = {20, 20, 20, 180};
        SDL_Color bord_off = {40, 40, 40, 200};
        SDL_Color txt_off  = {60, 60, 60, 255};
        rendu_rect_plein(r, b->rect, fond_off);
        rendu_rect_contour(r, b->rect, bord_off, 1);
        rendu_texte_centre(r, f, b->label,
            b->rect.x, b->rect.w,
            b->rect.y + b->rect.h/2 - 8, txt_off);
        return;
    }

    /* Fond du bouton si survole*/
    SDL_Color fond = {
        (Uint8)(b->survole ? b->couleur.r / 4 : b->couleur.r / 8),
        0, 0, 200
    };
    rendu_rect_plein(r, b->rect, fond);

    /* Bordure si survole */
    int intensite = b->survole ? 200 : 100;
    SDL_Color bord = {(Uint8)intensite, 0, 0, 255};
    rendu_rect_contour(r, b->rect, bord, b->survole ? 2 : 1);

    /* Effet glow si survoléer*/
    if (b->survole) {
        SDL_Color glow = {150, 0, 0, 30};
        SDL_Rect glow_rect = {b->rect.x - 2, b->rect.y - 2,
                              b->rect.w + 4, b->rect.h + 4};
        rendu_rect_contour(r, glow_rect, glow, 3);
    }

    /* Texte du bouton */
    SDL_Color txt_col = b->survole
        ? (SDL_Color){255, 80, 80, 255}
        : (SDL_Color){200, 40, 40, 255};
    rendu_texte_centre(r, f, b->label,
        b->rect.x, b->rect.w,
        b->rect.y + b->rect.h/2 - 8, txt_col);
}

//mise a jour etat bouton survoler
void rendu_bouton_update(Bouton *b, int mx, int my) {
    b->survole = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
                  my >= b->rect.y && my <= b->rect.y + b->rect.h) ? 1 : 0;
}

/* dtect clic */
int rendu_bouton_clique(Bouton *b, int mx, int my) {
    if (!b->actif) return 0;
    return (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
            my >= b->rect.y && my <= b->rect.y + b->rect.h) ? 1 : 0;
}

/* zone log */
void rendu_zone_log(SDL_Renderer *r, TTF_Font *f, ZoneLog *log,
                    int x, int y, int largeur, int hauteur) {
    /* Fond sombre*/
    SDL_Rect fond = {x, y, largeur, hauteur};
    SDL_Color fond_col = {5, 0, 0, 220};
    rendu_rect_plein(r, fond, fond_col);

    /* Bordure rouge sombre */
    SDL_Color bord = {80, 0, 0, 255};
    rendu_rect_contour(r, fond, bord, 1);

    /* LOG*/
    SDL_Color rouge_sombre = {120, 20, 10, 255};
    rendu_texte(r, f, "// LOG SYSTEME", x + 6, y - 35, rouge_sombre);

    /* Affiche les lignes */
    int ligne_h = 30;
    int max_visible = hauteur / ligne_h;
    int debut = (log->nb_lignes > max_visible)
                ? log->nb_lignes - max_visible : 0;

    for (int i = debut; i < log->nb_lignes; i++) {
        int iy = y + 4 + (i - debut) * ligne_h;
        /*rouge vif pour les nouvelles lignes, sombre sinon */
        SDL_Color col;
            if      (log->nouvelle[i] == 2) col = (SDL_Color){80, 120, 200, 255}; /* jaune = alerte  */
            else if (log->nouvelle[i] == 1) col = (SDL_Color){200,  60,  60, 255}; /* rouge = nouveau */
            else                            col = (SDL_Color){100,  20,  20, 255}; /* sombre = ancien */
        rendu_texte(r, f, log->lignes[i], x + 8, iy, col);
    }
}

/* ajout ligne log */
void log_ajouter(ZoneLog *log, const char *ligne, int est_nouvelle) {
    /* Si le log est plein -> decale toutes les lignes vers le haut */
    if (log->nb_lignes >= MAX_LIGNES_LOG) {
        for (int i = 0; i < MAX_LIGNES_LOG - 1; i++) {
            strncpy(log->lignes[i], log->lignes[i+1], 255);
            log->nouvelle[i] = log->nouvelle[i+1];
        }
        log->nb_lignes = MAX_LIGNES_LOG - 1;
    }
    strncpy(log->lignes[log->nb_lignes], ligne, 255);
    log->lignes[log->nb_lignes][255] = '\0';
    log->nouvelle[log->nb_lignes] = est_nouvelle;
    log->nb_lignes++;
}

/* Fond grille*/
void rendu_fond_grille(SDL_Renderer *r, int largeur, int hauteur) {
    /* Fond noir de base */
    SDL_SetRenderDrawColor(r, 5, 0, 0, 255);
    SDL_RenderClear(r);

    /* Lignes de grille  */
    SDL_SetRenderDrawColor(r, 35, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    int pas = 40;
    for (int x = 0; x < largeur; x += pas)
        SDL_RenderDrawLine(r, x, 0, x, hauteur);
    for (int y = 0; y < hauteur; y += pas)
        SDL_RenderDrawLine(r, 0, y, largeur, y);
}

/*Titre avec effet de halo glow*/
void rendu_titre_glow(SDL_Renderer *r, TTF_Font *f, const char *txt,
                      int x, int y, SDL_Color c) {
    if (!f) return;
    /* plusieur ligne pour faire glow */
    SDL_Color glow = {c.r, c.g, c.b, 40};
    for (int dx = -3; dx <= 3; dx++)
        for (int dy = -3; dy <= 3; dy++)
            if (dx != 0 || dy != 0)
                rendu_texte(r, f, txt, x + dx, y + dy, glow);
    /* Texte principal par-dessus */
    rendu_texte(r, f, txt, x, y, c);
}
