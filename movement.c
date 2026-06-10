#include "collisions.h"
#include "raylib.h"
#include "structs.h"
#include "movement.h"
#include "global-defs.h"
#include "enemies.h"
#include <stdio.h>


void SetGravity(Player *p, float gravity){
    p->acceleration.y=gravity;
}

void ApplyGravity(Player *p){
    p->velocity.y+=p->acceleration.y;
}

void HorizontalMovement(Player *p, int mat[MAT_HEIGHT][MAT_WIDTH]){
    int moveRight=(IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D));
    int moveLeft=(IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A));

    int moveDir=moveRight-moveLeft;

    


    if(p->velocity.x*moveDir>=MAX_VELOCITY){
        p->acceleration.x=0;
        p->velocity.x=moveDir*MAX_VELOCITY;
    }
    else{
        p->acceleration.x=moveDir*ACCEL;
    }

    if(moveDir==0){
        if(p->velocity.x>0){
            p->acceleration.x=-DEACCEL;
        }
        else if(p->velocity.x<0){
            p->acceleration.x=DEACCEL;
        }
        else {
            p->acceleration.x=0;
        }
        if(p->velocity.x!=0 && ((p->velocity.x * (p->velocity.x + p->acceleration.x))<0)){ //se velocidade mudar de sinal, parar de desacelerar
            p->acceleration.x=0;
            p->velocity.x=0;
        }
    }
}

void Damping(Player *p, int mat[MAT_HEIGHT][MAT_WIDTH]){

    int stairFlag = PlayerIsOnStair(p, mat);

    Player futureY = *p;
    futureY.position.y += p->velocity.y;

    if(!stairFlag){
        // Se bater no chão ou no teto
        if (PlayerIsOnGround(futureY, mat)) {
            if (p->velocity.y > 0) { 

                int blockI = ScreenYToMatrixLine(futureY.position.y + p->size.y);
                float blockTopY = MatrixPosToVec2(blockI, 0).y;
                

                p->position.y = blockTopY - p->size.y;
            } 
            else if (p->velocity.y < 0) { 

                int blockI = ScreenYToMatrixLine(futureY.position.y);
                float blockBottomY = MatrixPosToVec2(blockI, 0).y + SQR_SIZ;
                

                p->position.y = blockBottomY;
            }
            p->velocity.y = 0; 
        }
    }

    Player futureX = *p; 
    futureX.position.x += p->velocity.x;

    // Se bater na parede
    if (PlayerIsOnGround(futureX, mat)) {
        if (p->velocity.x > 0) { 

            int blockJ = ScreenXToMatrixColumn(futureX.position.x + p->size.x);
            float blockLeftX = MatrixPosToVec2(0, blockJ).x;
            
            p->position.x = blockLeftX - p->size.x;
        } 
        else if (p->velocity.x < 0) { 

            int blockJ = ScreenXToMatrixColumn(futureX.position.x);
            float blockRightX = MatrixPosToVec2(0, blockJ).x + SQR_SIZ;
            
            p->position.x = blockRightX;
        }
        p->velocity.x = 0;
        p->acceleration.x = 0;
    }
}


void Jump(Player *p){
    if(IsKeyDown(KEY_SPACE)){
        p->velocity.y=JUMP_VEL;
    }
}

void OnStairMove(Player *p, int flag){
    
    if(!flag){
        return;
    }
    
    int moveDown=(IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_W));
    int moveUp=(IsKeyDown(KEY_UP)||IsKeyDown(KEY_S));

    if(flag==4){
        moveUp=0;
    }
    else if(flag==1){
        moveDown=0;
    }
    else if(flag==3){
        //nn sei direito o q fazer
    }
    //no caso da flag 4, nn fazer nada

    int moveDir=moveDown-moveUp;


    p->velocity.y=moveDir*STAIR_VEL;

}

void Move(Player *p, int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager *e){

    int stairFlag=PlayerIsOnStair(p,mat);
    HorizontalMovement(p, mat);
    if(PlayerIsOnGround(*p,mat)||GroundBelow(p->position, p->size, mat)){
        Jump(p);
    }
    OnStairMove(p,stairFlag);
    if(!stairFlag){
        ApplyGravity(p);
    }
    int i;
    if(PlayerWillBeOnGround(*p, mat)){
        Damping(p, mat);
    }
    
    p->velocity.x+=p->acceleration.x;
    p->position.x+=p->velocity.x;
    p->position.y+=p->velocity.y;
    if(PlayerStompEnemy(*p, e, &i)){
        printf("Stomp\n");
        p->invincibilityFrames=10;
        KillEnemy(e, i);
        p->enemyScore+=100;
    }
    if(PlayerEnemyCollision(*p, e)){
        if(p->invincibilityFrames<=0){
            p->lives--;
            p->invincibilityFrames=60;
        }
        
    }
}