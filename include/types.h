#ifndef TYPES_H
#define TYPES_H

//Permet de def les struct et const partage par tous les fichers
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* Dimension fenetre jeu*/
#define FENETRE_LARGEUR  1200
#define FENETRE_HAUTEUR  900

//Nombre des neuds/aretes maximum
#define MAX_NOEUDS  70
#define MAX_ARETES  300

/* ─nb obj du stock */
#define NB_OBJETS  6

//capa max poids voiture
#define POIDS_MAX_VOITURE  60

//max de ligne dans la zone de log
#define MAX_LIGNES_LOG  50

/* palette de couleur possible */
#define COL_FOND          (SDL_Color){  5,   5,   5, 255}  /* noir profond        */
#define COL_FOND2         (SDL_Color){ 12,   0,   0, 255}  /* noir un peu rouge     */
#define COL_ROUGE_VIF     (SDL_Color){200,   0,   0, 255}  /* rouge principal     */
#define COL_ROUGE_SOMBRE  (SDL_Color){ 80,   0,   0, 255}  /* rouge fonce       */
#define COL_ROUGE_CLAIR   (SDL_Color){220,  60,  60, 255}  /* rouge texte         */
#define COL_ORANGE        (SDL_Color){204, 102,   0, 255}  /* accent orange       */
#define COL_GRIS_FONCE    (SDL_Color){ 40,  40,  40, 255}  /* gris      */
#define COL_GRIS          (SDL_Color){ 80,  80,  80, 255}  /* gris noeuds          */
#define COL_BLANC         (SDL_Color){255, 255, 255, 255}  /* texte blanc         */
#define COL_VOLEUR        (SDL_Color){220,  50,  50, 255}  /* rouge = voleur      */
#define COL_POLICE        (SDL_Color){ 50, 100, 220, 255}  /* bleu = police       */
#define COL_CAMERA_ON     (SDL_Color){255, 200,   0, 255}  /* jaune = camera ON   */
#define COL_CAMERA_OFF    (SDL_Color){ 60,  60,  60, 255}  /* gris = pas camera   */

/* etat de HACKNET */
typedef enum {
    ETAT_INTRO    = 0, 
    ETAT_STOCK    = 1,  /* modif stock     */
    ETAT_RESULTAT = 2,  /* rcahrgement voiture police     */
    ETAT_CAMERAS  = 3,  /* verif cam           */
    ETAT_JEU      = 4,  /* poursuite         */
    ETAT_GAME_OVER= 5,  /* si complice arrete       */
    ETAT_QUITTER  = 6   /* fermer l'application             */
} EtatJeu;

typedef struct {
    int   id;           /* identifiant    */
    float x, y;         /* position en pixels*/
    char  label[8];     /* nom */
    int   a_camera;     /* 1 si cam dessus   */
} Noeud;

typedef struct {
    int id_a;
    int id_b;
    int poids;
    int oriente;   /* 0 = bidirectionnel, 1 = oriente de a a b */
} Arete;

typedef struct {
    Noeud  noeuds[MAX_NOEUDS];
    Arete  aretes[MAX_ARETES];
    int    nb_noeuds;
    int    nb_aretes;
    //fait dans djikstra ou a*
    int    mat[MAX_NOEUDS][MAX_NOEUDS];

    int deb_voleur;
    int deb_police;
} Graphe;

typedef struct {
    char  nom[32];      /* nom*/
    int   quantite;     /* quantite act du stock*/
    int   poids;        /* poids en kg par unit        */
    float   bonus;        /* bonus/puissance pour la police   */
} ObjetStock;

/* Stock complet du commissariat  */
typedef struct {
    ObjetStock objets[NB_OBJETS];
    int        nb_objets;
} Stock;

/* bouton clicable SDL2 */
typedef struct {
    SDL_Rect  rect;         /* zone de clic/affichage           */
    char      label[64];    /* nom sur bouton              */
    int       survole;      /* 1 si la souris est dessus        */
    int       actif;        /* 1 si le bouton est utilisable    */
    SDL_Color couleur;      /* couleur          */
} Bouton;

/* zone log terminal jeu */
typedef struct {
    char lignes[MAX_LIGNES_LOG][256];
    int  nb_lignes;
    int  nouvelle[MAX_LIGNES_LOG];   /* 0=ancienne, 1=nouvelle, 2=police */
} ZoneLog;

/* un perso en mouv sur le graphe */
typedef struct {
    int   noeud_actuel;     /* noeud ou est le perso     */
    int   noeud_cible;      /* vers ou il va */
    float progress;         /* 0.0 a 1.0 sur l'arete parcouru */
    float x, y;             /* position en pixel*/
    int   est_ia;           /* 1 = controler par algo, 0 = joueur*/
} Entite;

/* infos globals jeu */
typedef struct {
    SDL_Window   *fenetre;
    SDL_Renderer *renderer;
    TTF_Font     *font_titre;
    TTF_Font     *font_normal;
    TTF_Font     *font_petit;
    TTF_Font     *font_sstitre;

    EtatJeu  etat;
    Graphe   graphe;
    Stock    stock;
    ZoneLog  log;
    Entite   voleur;
    Entite   police;

    int      ville_choisie;     /* 0=Ivry, 1=Laval, 2=Agen      */
    int      tour;              /* nbr de mouvement fait    */
    int      poids_voiture;     /* poids total pris par la police     */
    float      bonus_police;      /* bonus tot police   */
    int      jeu_en_cours;      /* 1 si la partie en cours*/
} Contexte;

#endif