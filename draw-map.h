#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include <stdlib.h>

#include "global-defs.h"
#include "structs.h"

/*
    FUNÇÃO QUE CARREGA OS MAPAS
*/

void LoadMapToMatrix(const char *, int [MAT_HEIGHT][MAT_WIDTH], EnemyManager*, Player*);
/* Dado um nome de arquivo de mapa, inicializa a matriz do mapa,
o player e o vetor de inimigos assim como a quantidade de inimigos no mapa*/



/*
    FUNÇÃO QUE DESENHA OS MAPAS
*/

void DrawMap(Player, EnemyManager, int [MAT_HEIGHT][MAT_WIDTH]);




/*
    FUNÇÃO QUE DESENHA TODAS AS TEXTURAS
*/

void LoadTextures(void);




extern Texture2D floorTex;

extern Texture2D hand_stairsTex;
