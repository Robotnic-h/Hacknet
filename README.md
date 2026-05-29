# 🦅 Falcon Eye – IA Tactique de Police

> Outil d'aide à la décision basé sur la théorie des graphes, permettant aux unités de police d'appréhender un suspect en fuite dans une zone métropolitaine.

---

## 📋 Description du projet

**Falcon Eye** est un système d'intelligence artificielle tactique composé de trois modules :

1. **Recherche du chemin le plus court** — navigation en temps réel vers le suspect
2. **Surveillance urbaine** — placement optimal de caméras de surveillance
3. **Logistique des véhicules** — sélection optimale d'équipements tactiques (problème du sac à dos)

---

## 📁 Structure du projet
/* A modifier en fonction des fichiers*/
/*Ici, il y a une dossier principal avec 2 fichiers à la racines et 3 sous dossiers (include, src pour source, obj pour l'executable qui va se créer à la compil)*/
```
falcon-eye/
├── README.md
├── Makefile             # Automatisation de la compilation
├── assets/              # Contient la police pour le visuel
│   └── font .ttf
├── include/             # Fichiers d'en-tête (.h)
│   ├── astar.h
│   ├── dijkstra.h
│   ├── ecran_camera.h
│   ├── ecran_intro.h
│   ├── ecran_jeu.h
│   ├── ecran_resultat.h
│   ├── ecran_stock.h
│   ├── Graph_AGEN.h
│   ├── Graph_LAVAL.h
│   ├── Graph_PARIS.h
│   ├── kruskal.h
│   ├── rendu.h
│   └── types.h
├── src/                 # Fichiers source (.c)
│   ├── astar.c
│   ├── dijkstra.c
│   ├── ecran_camera.c
│   ├── ecran_intro.c
│   ├── ecran_jeu.c
│   ├── ecran_resultat.c
│   ├── ecran_stock.c
│   ├── Graph_AGEN.c
│   ├── Graph_LAVAL.c
│   ├── Graph_PARIS.c
│   ├── kruskal.c
│   ├── main.c
│   └── rendu.c
└── obj/                 # Fichiers objets (générés lors de la compilation)
    ├── astar.o
    ├── dijkstra.o
    ├── ecran_camera.o
    ├── ecran_intro.o
    ├── ecran_jeu.o
    ├── ecran_resultat.o
    ├── ecran_stock.o
    ├── Graph_AGEN.o
    ├── Graph_LAVAL.o
    ├── Graph_PARIS.o
    ├── kruskal.o
    ├── main.o
    └── rendu.o
```

---

## ⚙️ Compilation

#### Attention 

Pour lancer le projet, il est nécessaire d'installer la librairie SDL2 avec les commandes suivantes

```
# Sur Linux

sudo apt update
sudo apt install libsdl2-dev

# Sur Windows avec vcpkg
# installer vcpkg via ce lien: https://vcpkg.io/en/?utm_source=chatgpt.com
vcpkg install sdl2

# Sur Windows avec MSYS2
pacman -S mingw-w64-x86_64-SDL2

# Vérifier l'installation
sdl2-config --version
```

### Prérequis

- Compilateur C (`gcc`)
- GNU Make (`make`)
- Bibliothèques mathématique standard (`-lm`)
- Et évidemment SDL2


### Utiliser le Makefile

```bash
# Compiler l'application
make

# recompiler l'application
make re

# Nettoyer les fichiers compilés
make clean

# Nettoyer et recompiler
make rebuild

# Afficher les informations de compilation
make help
```

### Compilation manuelle (alternative)

```bash
gcc -Wall -Wextra -g -Iinclude $(sdl2-config --cflags) \
  src/main.c src/dijkstra.c src/astar.c src/rendu.c \
  src/ecran_intro.c src/ecran_stock.c src/ecran_resultat.c \
  src/ecran_cameras.c src/ecran_jeu.c \
  src/Graph_LAVAL.c src/Graph_PARIS.c src/Graph_AGEN.c \
  src/kruskal.c \
  -o hacknet \
  $(sdl2-config --libs) -lSDL2_ttf -lm
```

---

## 🚀 Lancement

```bash
./hacknet
```

Le programme affiche un menu intéractif permettant de choisir le module à exécuter.
Vous êtes l'acolyte d'un voleur et êtes entrés dans le système de la police.

Votre but, aider votre collègue en fonction des différents modules à votre disposition.
Pour chaque module, il y a trois graphes représentant les villes de Laval, Agen et Paris.


---

## 🎒 Module 1 – Logistique des véhicules (Sac à dos 0/1)

### Problème

Chaque véhicule de police a une **capacité maximale de 60 kg**. Il faut sélectionner la combinaison d'équipements qui **maximise la valeur tactique totale** sans dépasser ce poids.

### Inventaire disponible

| Article               | Poids (kg) | Valeur Tactique (%) |
|-----------------------|------------|---------------------|
| Gyrophare             | 5          | 2                   |
| Menottes              | 2          | 0.5                 |
| Talkie Walkie         | 8          | 4                   |
| Brouilleur de signal  | 18         | 9                   |
| sifflet               | 1          | 07                  |
| Pistolet              | 10         | 5                   |

### Algorithme Best Fit

Nous avions commencé par implementer cet algorithme que nous connaissons depuis au moins notre 1er année à l'ESIEA. Une fois fait, nous nous sommes demandé si celui-ci était le plus efficace. 
Nous avons fait nos recherche et nous sommes rendu compte que knapsack etait plus opyimisé.

### Algorithme KNAPSACk

Programmation dynamique (sac à dos 0/1) — complexité O(n × W) où W = capacité maximale.

Structure: définit la structure Item (champs value, weight) et trois fonctions principales : knapsack(), Best_Fit() et main().

knapsack(int W, Item items[], int n) :

But: calculer la valeur maximale que l'on peut charger dans une capacité W (problème du sac à dos 0/1) et afficher les articles choisis.

Méthode: programmation dynamique avec une table 2D dp[n+1][W+1] où dp[i][w] = valeur maximale avec les i premiers articles et capacité w.
Remplissage: pour chaque article i et chaque capacité w, on choisit le max entre ne pas prendre l'article (dp[i-1][w]) ou le prendre (dp[i-1][w-wt] + val) si son poids wt ≤ w.
Traceback: après calcul, on remonte la table depuis dp[n][W] pour déterminer quels articles ont été pris, et on affiche la liste, le poids total et la valeur totale.
Complexité: temps O(n·W), mémoire O(n·W).
Best_Fit(int W, Item items[], int n) :

But: heuristique gloutonne simple (non optimale) pour remplir le sac.
Méthode: parcourt les articles dans l'ordre donné et ajoute chaque article si le poids cumulé reste ≤ W.
Complexité: O(n). Résultat dépend de l'ordre des articles (pas de tri effectué).
main() :

Définit W = 40 et un tableau d'Item.
Appelle knapsack() pour afficher la valeur optimale et les articles choisis, puis Best_Fit() pour afficher la valeur obtenue par l'heuristique.
Observations:

knapsack() fournit la solution exacte (DP) mais coûteuse en mémoire pour grandes capacités.
Best_Fit() est rapide mais souvent sous-optimal.
Le code imprime à la fois la valeur optimale et la solution heuristique pour comparaison.

### Références
https://www.geeksforgeeks.org/dsa/0-1-knapsack-problem-dp-10/
---

## 📷 Module 2 – Surveillance urbaine

### Problème

Minimiser le nombre de caméras à placer aux intersections de façon à ce que **chaque rue soit surveillée** par au moins une caméra. Une caméra placée à un nœud couvre toutes les arêtes directement connectées.

Ce problème est connu sous le nom de **couverture minimale par sommets** (*Minimum Vertex Cover*).

### Approche implémentée

Heuristique gloutonne :
1. Sélectionner le nœud de plus haut degré (le plus de rues connectées)
2. Marquer toutes ses arêtes comme couvertes
3. Répéter jusqu'à couverture totale

Entrée / Sortie: prend une matrice d'adjacence pondérée n×n ; renvoie/affiche la liste des intersections à équiper, le nombre de caméras et le coût total (somme des poids de l'ACM).
Étape 1 — Extraction et tri des arêtes: lit la matrice, crée la liste des arêtes (u,v,poids) puis trie par poids croissant (préparation pour Kruskal).

Étape 2 — Kruskal + DSU: utilise une structure Union-Find (DSU) pour construire l'Arbre Couvrant Minimum (ACM). Complexité dominante: tri O(E log E); DSU avec compression donne quasi-constante par opération.

Étape 3 — Calcul des degrés: calcule le degré de chaque sommet dans l'ACM (compte des arêtes incidentes).

Étape 4 — Vertex cover glouton sur l'ACM: parcourt les arêtes de l'ACM, et pour chaque arête non encore couverte, choisit le sommet (u ou v) de plus grand degré, place une caméra à ce sommet, et marque toutes les arêtes incidentes comme couvertes. C'est une heuristique gloutonne (rapide mais pas garantie optimale pour le vertex cover général).
Résultat: res->nb_cameras (nombre de caméras), res->cameras[] (liste des sommets choisis), res->cout_total (coût total de l'ACM). Le programme affiche ensuite ces informations.
Observations / limites:

L'approche réduit le problème global (vertex cover) à une heuristique sur l'ACM — résultat pratique mais pas nécessairement minimal globalement.
Le coût affiché est le coût de l'ACM (somme des poids), pas un coût lié aux caméras elles‑mêmes.

Complexité globale: tri + Kruskal O(E log E) + O(E) pour le reste; mémoire proportionnelle à E et n.

### Réfférences
- https://github.com/vikasawadhiya/Kruskal-Algorithm
- https://www.geeksforgeeks.org/dsa/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/

---


## 🗺️ Module 3 – Chemin le plus court (A\*)

### Principe

Le réseau routier de la ville est modélisé comme un **graphe orienté pondéré** :

- **Nœuds** : intersections de la ville
- **Arêtes** : rues reliant deux intersections
- **Poids** : coût de traversée d'une rue (distance, vitesse, densité du trafic)

### Entrée

Le graphe est défini par une **matrice d'adjacence** `int matrice[N][N]` :

```c
int matrice[N][N] = {
/*       0   1   2   3   4   5   6   7   8   9  */
/* 0 */ { 0,  2,  0,  5,  8,  0,  0,  0,  0,  0 },
/* 1 */ { 2,  0,  4,  0,  6,  0,  0,  0,  0,  0 },
/* 2 */ { 0,  4,  0,  0,  0,  3,  7,  0,  0,  0 },
/* 3 */ { 5,  0,  0,  0,  3,  0,  0,  9,  0,  0 },
/* 4 */ { 8,  6,  0,  3,  0,  2,  0,  4,  6,  0 },
/* 5 */ { 0,  0,  3,  0,  2,  0,  5,  0,  3,  7 },
/* 6 */ { 0,  0,  7,  0,  0,  5,  0,  0,  0,  4 },
/* 7 */ { 0,  0,  0,  9,  4,  0,  0,  0,  5,  0 },
/* 8 */ { 0,  0,  0,  0,  6,  3,  0,  5,  0,  2 },
/* 9 */ { 0,  0,  0,  0,  0,  7,  4,  0,  2,  0 },
};
```

`0` = pas d'arête, `> 0` = poids de l'arête.

###Djikstra

Nous avion implenté pour commencer Djikstra qui fonctionne avec la matrice d'adjacence. Le code est toujours dans le dossier.

Nous savons que Djikstra est un bon algorithme de plus court chemin, mais en termes d'efficacité A* le surpasse, c'est d'ailleur une variante de celui-ci qui est souvent utilisé dans nos GPS.

Pour pouvoir nous renseigner nous avons chercher un algorithme A* en C sans IA, mais sans succès, notre seul source d'information est un algorithme en C++, et les algorithmes en language naturel de nos cours..

### Algorithme A\*

L'algorithmeA\* combine :
- `g(n)` : coût réel depuis la source
- `h(n)` : heuristique (distance euclidienne vers la destination)
- `f(n) = g(n) + h(n)` : score de priorité

À chaque étape, le nœud avec le **f minimum** est développé. L'heuristique étant admissible, le chemin retourné est **toujours optimal**.

> Si les coordonnées ne sont pas disponibles, mettre `x[] = y[] = 0` → A\* se comporte alors comme Dijkstra.

### Simulation dynamique

L'hélicoptère transmet en temps réel la position du voleur et de la police. À chaque mise à jour :

1. Le nœud cible est mis à jour
2. `astar()` est rappelée depuis la position courante du véhicule
3. Le nouveau trajet est affiché

### Exemple de sortie

```
=== Falcon Eye – Algorithme A* ===

Test 1 : 0 --> 9
Chemin trouvé (6 nœuds) :
  0 --(2.0)--> 1 --(4.0)--> 2 --(3.0)--> 5 --(3.0)--> 8 --(2.0)--> 9
Coût total : 14.00

Test 3 : 3 --> 9
Chemin trouvé (5 nœuds) :
  3 --(3.0)--> 4 --(2.0)--> 5 --(3.0)--> 8 --(2.0)--> 9
Coût total : 10.00
```

---

## 👥 Équipe

Ce projet est réalisé par un groupes de 3 étudiants dans le cadre du cours de **Théorie des Graphes**.
Rose Nédélec
Jordan Philippe
Amaury Lefevre
