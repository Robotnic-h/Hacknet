CC      = gcc
TARGET  = hacknet
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

CFLAGS  = -Wall -Wextra -g \
          -I$(INC_DIR) \
          $(shell sdl2-config --cflags)

LDFLAGS = $(shell sdl2-config --libs) \
          -lSDL2_ttf \
          -lm

SRCS = $(SRC_DIR)/main.c           \
       $(SRC_DIR)/dijkstra.c       \
       $(SRC_DIR)/astar.c          \
       $(SRC_DIR)/rendu.c          \
       $(SRC_DIR)/ecran_intro.c    \
       $(SRC_DIR)/ecran_stock.c    \
       $(SRC_DIR)/ecran_resultat.c \
       $(SRC_DIR)/ecran_cameras.c  \
       $(SRC_DIR)/ecran_jeu.c      \
       $(SRC_DIR)/Graph_LAVAL.c    \
       $(SRC_DIR)/Graph_PARIS.c    \
       $(SRC_DIR)/Graph_AGEN.c     \
       $(SRC_DIR)/kruskal.c

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo ""
	@echo "  [OK] Compilation reussie → ./$(TARGET)"
	@echo ""

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "  [OK] Nettoyage termine"

re: clean all

.PHONY: all run clean re