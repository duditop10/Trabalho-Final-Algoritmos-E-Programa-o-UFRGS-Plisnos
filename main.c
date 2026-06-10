#include "raylib.h"
#include "structs.h"
#include <string.h>
#include "global-defs.h"
#include "draw-map.h"
#include "movement.h"
#include "enemies.h"
#include "collisions.h"
#include <time.h> 
#include <stdio.h>

int main(){
    int mat[MAT_HEIGHT][MAT_WIDTH]={0};
    EnemyManager enemyMan={0};
    Player p = {0};
    p.lives=3;

    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Donkey Kong");

    SetRandomSeed((unsigned int)time(NULL)); 

    LoadMapToMatrix("mapa2.txt", mat, &enemyMan, &p);
    LoadTextures();

    SetTargetFPS(60);               
    // Main game loop
    time_t initTime=time(NULL);
    
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        if(p.invincibilityFrames>0){
            p.invincibilityFrames--;
        }
        SetGravity(&p, 9.8/60);
        Move(&p, mat, &enemyMan);
        if(OutOfBoundsCheck(p.position, p.size)){
            p.lives=0;
        }
        UpdateEnemies(&enemyMan, mat);

        printf("Lives: %d\n", p.lives);
        
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawMap(p, enemyMan, mat);

        EndDrawing();

        if(p.lives<=0){
            time_t timeElapsed=time(NULL)-initTime;
            p.timeScore=10000/((unsigned int)timeElapsed+1);
            p.score=p.timeScore+p.enemyScore;
            printf("%u|%u|%u\n", p.timeScore, p.enemyScore, p.score);
            break;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
