#ifndef RENDU_H
#define RENDU_H

/* Fct SDL2 pour dessiner sur tous les ecrans */

#include "types.h"

int  rendu_init(Contexte *ctx);
void rendu_quitter(Contexte *ctx);

/* fait un rect rempli avec couleur choisi */
void rendu_rect_plein(SDL_Renderer *r, SDL_Rect rect, SDL_Color c);

//fait le contour d'un rectangle
void rendu_rect_contour(SDL_Renderer *r, SDL_Rect rect, SDL_Color c, int epaisseur);

//Faire un cercle pleins
void rendu_cercle(SDL_Renderer *r, int cx, int cy, int rayon, SDL_Color c);

/* imprimer contour d'un cercle*/
void rendu_cercle_contour(SDL_Renderer *r, int cx, int cy, int rayon, SDL_Color c);

/* relie 2 points avec une ligne epaisse */
void rendu_ligne(SDL_Renderer *r, int x1, int y1, int x2, int y2, SDL_Color c, int epaisseur);

/* dessine une fleche pour aretes oriente */
void rendu_fleche(SDL_Renderer *r, int x1, int y1, int x2, int y2, SDL_Color c, int epaisseur);

/* ecrire texte a une position x y */
void rendu_texte(SDL_Renderer *r, TTF_Font *f, const char *txt, int x, int y, SDL_Color c);

/* ecrit un texte centrer */
void rendu_texte_centre(SDL_Renderer *r, TTF_Font *f, const char *txt, int zone_x, int zone_largeur, int y, SDL_Color c);

/* Ddessine un bouton avec son nom */
void rendu_bouton(SDL_Renderer *r, TTF_Font *f, Bouton *b);

//met a jour si un bouton est survole 
void rendu_bouton_update(Bouton *b, int mx, int my);

/* indique si le clic est fait sur un bouton */
int rendu_bouton_clique(Bouton *b, int mx, int my);

/* dessine la zone log */
void rendu_zone_log(SDL_Renderer *r, TTF_Font *f, ZoneLog *log, int x, int y, int largeur, int hauteur);

/* rajoute une ligne au log et supprime si besoin */
void log_ajouter(ZoneLog *log, const char *ligne, int est_nouvelle);

//Dessine un fond style grille 
void rendu_fond_grille(SDL_Renderer *r, int largeur, int hauteur);

/* Dessine un titre glowy */
void rendu_titre_glow(SDL_Renderer *r, TTF_Font *f, const char *txt, int x, int y, SDL_Color c);

#endif