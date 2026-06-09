#include "draw-map.h"
#include "structs.h"
#include "global-defs.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "collisions.h"

/*
    AS FUN��ES AQUI CARREGAM OS MAPAS, MODIFICAM A GRID E DESENHAM OS OBJETOS
    TAMB�M � ONDE DEFINIMOS COISAS IMPORTANTES PARA O MAPA. COMO A GRID DO MAPA E O VETOR COM OS ANDARES
*/




/*
    FUN��O QUE CARREGA OS MAPAS
*/


void LoadMapToMatrix(const char *filename, int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager* enemyMan, Player *p)
{
    FILE *arquivo = fopen(filename, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir %s\n", filename);
        exit(1);
    }
    int i=0;
    char linha[MAT_WIDTH + 3]; //Tem isso pois o arquivo l� uma linha inteira por vez. Tem o mais dois para pegar o \n que � CD LF ou alguma coisa do g�nero no arquivo oficial
                                //bote mais 3 pra contar com o \0 tbm
                                //eu acho que daria pra resolver usando getline mas assim ta bom por enquanto. ASS: Gian
    for (int y = 0; y < MAT_HEIGHT; y++)
    {
        fgets(linha, sizeof(linha), arquivo);


        for (int x = 0; x < MAT_WIDTH; x++)
        {
            switch (linha[x])
            {
                case ' ':
                case '.': //pra debug
                    mat[y][x] = 0;
                    break;

                case 'Z':
                case 'z': //vai que né? nn sabemos se a sora vai botar letras minúsculas
                    mat[y][x] = 1;
                    break;

                case 'H':
                case 'h':
                    mat[y][x] = 2;
                    break;

                case 'E':
                case 'e':
                    mat[y][x]=0;
                    enemyMan->enemies[i].alive=1;
                    enemyMan->enemies[i].position=MatrixPosToVec2(y,x);
                    enemyMan->enemies[i].size=(Vector2){(float)SQR_SIZ,(float)SQR_SIZ};
                    enemyMan->enemies[i].canClimbLadders=(GetRandomValue(0, 100) < 100); //20% das vezes, 80% não
                    i++;
                    break;

                case 'S':
                case 's':
                    mat[y][x] = 3;
                    break;

                case 'D':
                case 'd':
                    mat[y][x] = 4;
                    break;

                case 'P':
                case 'p':
                    mat[y][x]=0;
                    p->position=MatrixPosToVec2(y,x);
                    p->size.x=SQR_SIZ;
                    p->size.y=SQR_SIZ;
                    break;

                case 'F':
                case 'f':
                    mat[y][x] = 5;
                    break;
            }
        }
    }
    enemyMan->quantEnemies=i;
    fclose(arquivo);
}


/*
    FUN��O QUE DESENHA OS MAPAS
*/

void DrawMap(Player p, EnemyManager enemyMan, int mat[MAT_HEIGHT][MAT_WIDTH]){

    int block;


    for(int i = 0; i < MAT_HEIGHT; i++){
        for(int j = 0; j < MAT_WIDTH; j++){

            block = mat[i][j];

            Vector2 posBlock=MatrixPosToVec2(i,j);

            switch(block){
                case 0: //empty
                    break;
                case 1:     //Blocos do mapa
                    DrawTexture(floorTex, posBlock.x, posBlock.y, WHITE);
                    break;
                case 2:     //Escada
                case 3:
                    DrawTexture(hand_stairsTex, posBlock.x, posBlock.y, WHITE);
                    break;
                case 4:
                    //N�o � desenhada pois a escada teria (no desenho, esse ponto � importante para a movimenta��o na escada) um grid a mais do que deveria
                    break;
                case 5:
                    DrawRectangle(posBlock.x, posBlock.y, SQR_SIZ, SQR_SIZ, COLOR_GOAL);
                    break;
            }
        }
    }

    /*
        DESENHA OS INIMIGOS
    */
    for(int i = 0; i < enemyMan.quantEnemies; i++){
        DrawRectangleV(enemyMan.enemies[i].position, enemyMan.enemies[i].size, COLOR_ENEMY);
    }

    /*
        DESENHA O PLAYER
    */
    DrawRectangleV(p.position, p.size, COLOR_PLAYER);

}


Texture2D floorTex;
Texture2D hand_stairsTex;

/*
    FUN��O QUE DESENHA TODAS AS TEXTURAS
*/

void LoadTextures(void)
{
    Image floorImg = LoadImage("Floor.png");

    if (floorImg.data == NULL)
    {
        printf("Erro ao carregar Floor.png\n");
        return;
    }

    ImageResize(&floorImg, SQR_SIZ, SQR_SIZ);

    floorTex = LoadTextureFromImage(floorImg);

    UnloadImage(floorImg);

    Image stairsImg = LoadImage("HandStairs.png");

    if (stairsImg.data == NULL)
    {
        printf("Erro ao carregar HandStairs.png\n");
        return;
    }

    ImageResize(&stairsImg, SQR_SIZ, SQR_SIZ);

    hand_stairsTex = LoadTextureFromImage(stairsImg);

    UnloadImage(stairsImg);
}
