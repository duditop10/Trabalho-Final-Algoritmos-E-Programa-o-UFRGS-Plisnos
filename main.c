#include "raylib.h"
#include "structs.h"
#include <string.h>
#include "global-defs.h"
#include "draw-map.h"
#include "movement.h"
#include "enemies.h"
#include <time.h> 
#include <stdio.h>

int main(){
    int mat[MAT_HEIGHT][MAT_WIDTH]={0};
    EnemyManager enemyMan={0};
    Player p = {0};
    p.lives=3;

    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Donkey Kong");

    SetRandomSeed((unsigned int)time(NULL)); 

    LoadMapToMatrix("mapa1.txt", mat, &enemyMan, &p);
    LoadTextures();

    SetTargetFPS(60);               

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        SetGravity(&p, 9.8/60);
        Move(&p, mat, &enemyMan);
        UpdateEnemies(&enemyMan, mat);
        printf("%d",p.lives);
        
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawMap(p, enemyMan, mat);

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
