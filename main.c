#include "raylib.h"
#include "structs.h"
#include <string.h>
#include "global-defs.h"
#include "draw-map.h"

#define MAX_VELOCITY 3.7

int main(){
    int mat[MAT_HEIGHT][MAT_WIDTH]={0};
    EnemyManager enemyMan={0};
    Player p = {0};
    p.lives=3;
    LoadMapToMatrix("mapa0.txt", mat, enemyMan, p);
    LoadTextures();

    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Donkey Kong");

    SetTargetFPS(60);               

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        

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

int OutOfBoundsCheck(Player* p){

    int outOfBounds = 0; //valor de retorno, 0 se ele vai estar out of bounds, 1 se estará out of bounds,
    // 2 se estará out of bounds pelo down check, (para fins de check de pulo)

    int upBoundCheck= (*p).position.y + (*p).velocity.y < 0;
    int downBoundCheck= (*p).position.y + (*p).size.y + (*p).velocity.y > SCR_HEIGHT;

    if(upBoundCheck){
        (*p).velocity.y = 0;
        (*p).position.y = 0;
        outOfBounds=1;
    }
    else if(downBoundCheck){
        (*p).velocity.y = 0;
        (*p).position.y = SCR_HEIGHT - (*p).size.y;
        outOfBounds=2;
    }

    int leftBoundCheck = (*p).position.x + (*p).velocity.x < 0;
    int rightBoundCheck = (*p).position.x + (*p).size.x + (*p).velocity.x > SCR_WIDTH;
    
    if(leftBoundCheck){
        (*p).velocity.x = 0;
        (*p).position.x = 0;
        if(!outOfBounds){
            outOfBounds=1;
        }
    }
    else if(rightBoundCheck){
        (*p).velocity.x = 0;
        (*p).position.x = SCR_WIDTH - (*p).size.x;
        if(!outOfBounds){
            outOfBounds=1;
        }
    }
    return outOfBounds;
}

void MovementInput(Player* p){
    int leftOrRightPressedThisFrame=0;

    float acceleration=0.1;
    float deceleration=-0.2;
    float jumpVel=-2.5;
    if(GroundCollisionCheck(p)){
        if(IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)){
            if((*p).velocity.x>=MAX_VELOCITY){
                (*p).acceleration.x=0;
                (*p).velocity.x=MAX_VELOCITY;
            }
            else{
                (*p).acceleration.x=acceleration;
            }
            leftOrRightPressedThisFrame=1;
        }
        if(IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)){
            if((*p).velocity.x<=-MAX_VELOCITY){
                (*p).acceleration.x=0;
                (*p).velocity.x=-MAX_VELOCITY;
            }
            else{
                (*p).acceleration.x=-acceleration;
            }
            leftOrRightPressedThisFrame=1;
        }
        if(!leftOrRightPressedThisFrame){
            if((*p).velocity.x>0){
                (*p).acceleration.x=deceleration;
            }
            else if((*p).velocity.x<0){
                (*p).acceleration.x=-deceleration;
            }
            else {
                (*p).acceleration.x=0;
            }
            if(((*p).velocity.x+(*p).acceleration.x)/(*p).velocity.x<0){
                (*p).acceleration.x=0;
                (*p).velocity.x=0;
            }
        } 
        
        if(IsKeyDown(KEY_SPACE)){
            (*p).velocity.y=-4;
        }
        (*p).velocity.x+=(*p).acceleration.x;
        if (!IsGroundWithinJumpRange(p)){
            (*p).velocity.x=0;
            (*p).acceleration.x=0;
        }
        else{
            (*p).velocity.x-=(*p).acceleration.x;
        }
    }
    else{
        (*p).acceleration.x=0;
        if (!IsGroundWithinJumpRange(p)){
            (*p).velocity.x=0;
        }
    }
    
}



