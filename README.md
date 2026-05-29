# 🦅 Falcon Eye – Tactical Police AI

> A decision-support tool based on graph theory, enabling police units to apprehend a fleeing suspect in a metropolitan area.

---

## 📋 Project Description

**Falcon Eye** is a tactical artificial intelligence system composed of three modules:

1. **Shortest path search** — real-time navigation toward the suspect
2. **Urban surveillance** — optimal placement of surveillance cameras
3. **Vehicle logistics** — optimal selection of tactical equipment (knapsack problem)

---

## 📁 Project Structure

```
falcon-eye/
├── README.md
├── Makefile             # Build automation
├── assets/              # Contains the font for visuals
│   └── font.ttf
├── include/             # Header files (.h)
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
├── src/                 # Source files (.c)
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
└── obj/                 # Object files (generated during compilation)
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

#### Important

To run the project, you must install the SDL2 library using the following commands:

```bash
# On Linux
sudo apt update
sudo apt install libsdl2-dev

# On Windows with vcpkg
# Install vcpkg via this link: https://vcpkg.io/en/
vcpkg install sdl2

# On Windows with MSYS2
pacman -S mingw-w64-x86_64-SDL2

# Verify installation
sdl2-config --version
```

### Prerequisites

- C compiler (`gcc`)
- GNU Make (`make`)
- Standard math library (`-lm`)
- SDL2

### Using the Makefile

```bash
# Compile the application
make

# Recompile the application
make re

# Clean compiled files
make clean

# Clean and recompile
make rebuild

# Display compilation information
make help
```

### Manual compilation (alternative)

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

## 🚀 Running the Program

```bash
./hacknet
```

The program displays an interactive menu for selecting which module to run. You play the role of a thief's accomplice who has broken into the police system.

Your goal is to help your colleague using the various modules at your disposal. Each module offers three graphs representing the cities of Laval, Agen, and Paris.

---

## 🎒 Module 1 – Vehicle Logistics (0/1 Knapsack)

### Problem

Each police vehicle has a **maximum capacity of 60 kg**. The goal is to select the combination of equipment that **maximizes total tactical value** without exceeding this weight.

### Available Inventory

| Item                  | Weight (kg) | Tactical Value (%) |
|-----------------------|-------------|---------------------|
| Emergency light       | 5           | 2                   |
| Handcuffs             | 2           | 0.5                 |
| Walkie-Talkie         | 8           | 4                   |
| Signal jammer         | 18          | 9                   |
| Whistle               | 1           | 7                   |
| Pistol                | 10          | 5                   |

### Best Fit Algorithm

We initially implemented this algorithm, which we have been familiar with since at least our first year at ESIEA. Once completed, we questioned whether it was the most efficient approach. After further research, we concluded that the Knapsack algorithm was better optimized.

### KNAPSACK Algorithm

Dynamic programming (0/1 knapsack) — complexity O(n × W) where W = maximum capacity.

**Structure:** defines the `Item` struct (fields `value`, `weight`) and three main functions: `knapsack()`, `Best_Fit()`, and `main()`.

**`knapsack(int W, Item items[], int n)`:**

- **Purpose:** compute the maximum value that can be loaded within capacity W (0/1 knapsack problem) and display the chosen items.
- **Method:** dynamic programming using a 2D table `dp[n+1][W+1]` where `dp[i][w]` = maximum value using the first `i` items with capacity `w`.
- **Filling:** for each item `i` and each capacity `w`, choose the max between not taking the item (`dp[i-1][w]`) or taking it (`dp[i-1][w-wt] + val`) if its weight `wt ≤ w`.
- **Traceback:** after computation, backtrack through the table from `dp[n][W]` to determine which items were selected, then display the list, total weight, and total value.
- **Complexity:** time O(n·W), memory O(n·W).

**`Best_Fit(int W, Item items[], int n)`:**

- **Purpose:** simple greedy heuristic (non-optimal) for filling the knapsack.
- **Method:** iterates through items in the given order and adds each one if the cumulative weight stays ≤ W.
- **Complexity:** O(n). Result depends on item ordering (no sorting performed).

**`main()`:** sets W = 40 and an array of `Item`, calls `knapsack()` to display the optimal value and selected items, then calls `Best_Fit()` to display the heuristic result.

**Observations:**
- `knapsack()` provides the exact solution (DP) but is memory-intensive for large capacities.
- `Best_Fit()` is fast but often suboptimal.
- The code prints both the optimal value and the heuristic solution for comparison.

### References
- https://www.geeksforgeeks.org/dsa/0-1-knapsack-problem-dp-10/

---

## 📷 Module 2 – Urban Surveillance

### Problem

Minimize the number of cameras placed at intersections such that **every street is monitored** by at least one camera. A camera placed at a node covers all directly connected edges.

This problem is known as **Minimum Vertex Cover**.

### Implemented Approach

Greedy heuristic:
1. Select the node with the highest degree (most connected streets)
2. Mark all its edges as covered
3. Repeat until full coverage

**Input / Output:** takes a weighted n×n adjacency matrix; returns/displays the list of intersections to equip, the number of cameras, and the total cost (sum of MST weights).

**Step 1 — Edge extraction and sorting:** reads the matrix, builds the edge list (u, v, weight), then sorts by ascending weight (preparation for Kruskal).

**Step 2 — Kruskal + DSU:** uses a Union-Find (DSU) structure to build the Minimum Spanning Tree (MST). Dominant complexity: sorting O(E log E); DSU with path compression gives near-constant time per operation.

**Step 3 — Degree computation:** computes each vertex's degree in the MST (count of incident edges).

**Step 4 — Greedy vertex cover on MST:** iterates over MST edges; for each uncovered edge, selects the vertex (u or v) with the highest degree, places a camera there, and marks all incident edges as covered. This is a greedy heuristic (fast but not guaranteed optimal for the general vertex cover problem).

**Result:** `res->nb_cameras` (number of cameras), `res->cameras[]` (list of selected vertices), `res->cout_total` (total MST cost). The program then displays this information.

**Observations / limitations:**
- The approach reduces the global problem (vertex cover) to a heuristic on the MST — practical result, but not necessarily globally minimal.
- The displayed cost is the MST cost (sum of weights), not a camera-related cost.
- **Overall complexity:** sorting + Kruskal O(E log E) + O(E) for the rest; memory proportional to E and n.

### References
- https://github.com/vikasawadhiya/Kruskal-Algorithm
- https://www.geeksforgeeks.org/dsa/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/

---

## 🗺️ Module 3 – Shortest Path (A\*)

### Principle

The city's road network is modeled as a **weighted directed graph**:

- **Nodes:** city intersections
- **Edges:** streets connecting two intersections
- **Weights:** traversal cost of a street (distance, speed, traffic density)

### Input

The graph is defined by an **adjacency matrix** `int matrice[N][N]`:

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

`0` = no edge, `> 0` = edge weight.

### Dijkstra

We initially implemented Dijkstra, which works with the adjacency matrix. The code is still in the project folder.

We know Dijkstra is a solid shortest-path algorithm, but in terms of efficiency, A* surpasses it — a variant of A* is in fact commonly used in GPS systems.

To learn more, we searched for an A* implementation in C without AI assistance, but without success. Our only reference was an implementation in C++, along with the natural-language algorithm descriptions from our coursework.

### A\* Algorithm

The A\* algorithm combines:
- `g(n)`: actual cost from the source
- `h(n)`: heuristic (Euclidean distance to the destination)
- `f(n) = g(n) + h(n)`: priority score

At each step, the node with the **minimum f** is expanded. Since the heuristic is admissible, the returned path is **always optimal**.

> If coordinates are unavailable, set `x[] = y[] = 0` → A\* then behaves like Dijkstra.

### Dynamic Simulation

The helicopter transmits the thief's and police's positions in real time. On each update:

1. The target node is updated
2. `astar()` is called again from the vehicle's current position
3. The new route is displayed

### Sample Output

```
=== Falcon Eye – A* Algorithm ===

Test 1 : 0 --> 9
Path found (6 nodes):
  0 --(2.0)--> 1 --(4.0)--> 2 --(3.0)--> 5 --(3.0)--> 8 --(2.0)--> 9
Total cost: 14.00

Test 3 : 3 --> 9
Path found (5 nodes):
  3 --(3.0)--> 4 --(2.0)--> 5 --(3.0)--> 8 --(2.0)--> 9
Total cost: 10.00
```

---

## 👥 Team

This project was carried out by a group of 3 students as part of the **Graph Theory** course.

Rose Nédélec
Jordan Philippe
Amaury Lefevre
