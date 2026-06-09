#include "collisions.h"
#include "raylib.h"
#include "structs.h"
#include "movement.h"
#include "global-defs.h"
#include "enemies.h"



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
    // --- 1. RESOLUÇÃO DO EIXO Y (Vertical) ---
    Player futureY = *p;
    futureY.position.y += p->velocity.y;

    // Se bater no chão ou no teto
    if (PlayerIsOnGround(futureY, mat)) {
        if (p->velocity.y > 0) { 
            // Caindo (bateu no chão): descobre a linha da matriz onde o PÉ bateu
            int blockI = ScreenYToMatrixLine(futureY.position.y + p->size.y);
            float blockTopY = MatrixPosToVec2(blockI, 0).y;
            
            // Gruda o jogador exatamente em cima da superfície do bloco
            p->position.y = blockTopY - p->size.y;
        } 
        else if (p->velocity.y < 0) { 
            // Subindo (bateu no teto): descobre a linha da matriz onde a CABEÇA bateu
            int blockI = ScreenYToMatrixLine(futureY.position.y);
            float blockBottomY = MatrixPosToVec2(blockI, 0).y + SQR_SIZ;
            
            // Gruda o jogador exatamente embaixo do bloco
            p->position.y = blockBottomY;
        }
        p->velocity.y = 0; // Bateu, então zera SOMENTE a velocidade vertical
    }

    // --- 2. RESOLUÇÃO DO EIXO X (Horizontal) ---
    // Cria um futuro X partindo da posição Y já corrigida do passo anterior
    Player futureX = *p; 
    futureX.position.x += p->velocity.x;

    // Se bater na parede
    if (PlayerIsOnGround(futureX, mat)) {
        if (p->velocity.x > 0) { 
            // Indo para a direita: pega a coluna da matriz onde o LADO DIREITO bateu
            int blockJ = ScreenXToMatrixColumn(futureX.position.x + p->size.x);
            float blockLeftX = MatrixPosToVec2(0, blockJ).x;
            
            p->position.x = blockLeftX - p->size.x;
        } 
        else if (p->velocity.x < 0) { 
            // Indo para a esquerda: pega a coluna da matriz onde o LADO ESQUERDO bateu
            int blockJ = ScreenXToMatrixColumn(futureX.position.x);
            float blockRightX = MatrixPosToVec2(0, blockJ).x + SQR_SIZ;
            
            p->position.x = blockRightX;
        }
        p->velocity.x = 0; // Bateu na parede, zera a velocidade horizontal
        p->acceleration.x = 0; // Evita que a física continue forçando aceleração contra o bloco
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

    HorizontalMovement(p, mat);
    if(PlayerIsOnGround(*p,mat)||GroundBelow(p->position, p->size, mat)){
        Jump(p);
    }
    int stairFlag=PlayerIsOnStair(p,mat);
    OnStairMove(p,stairFlag);
    if(!stairFlag){
        ApplyGravity(p);
    }
    if(PlayerWillBeOnGround(*p, mat)){
        Damping(p, mat);
    }
    int i;
    if(PlayerStompEnemy(*p, e, &i)){
        KillEnemy(e, i);
    }
    p->velocity.x+=p->acceleration.x;
    p->position.x+=p->velocity.x;
    p->position.y+=p->velocity.y;
    if(PlayerEnemyCollision(*p, e)){
        p->lives--;
    }
}