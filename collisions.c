#include "raylib.h"
#include "global-defs.h"
#include "structs.h"
#include "collisions.h"

int ScreenYToMatrixLine(float y){
    int i;
    y-=SCR_HEIGHT-SCR_WIDTH; //contagem começa logo depois dos pixeis extra para score/hud
    i=(int)(y/(float)SQR_SIZ);
    return i;
}
int ScreenXToMatrixColumn(float x){
    int j;
    j=(int)(x/(float)SQR_SIZ);
    return j;
}
Vector2 MatrixPosToVec2(int i, int j){
    int yOffset=SCR_HEIGHT-SCR_WIDTH; // starts at this point to make the window essentially square
    int size = SCR_WIDTH/MAT_WIDTH;

    Vector2 result;
    result.x=(float)j*size;
    result.y=(float)i*size+yOffset;
    return result;
}

int CheckCollisionVec2Vec2(Vector2 posSelf, Vector2 sizeSelf, Vector2 posOther, Vector2 sizeOther){
    return (posSelf.x < posOther.x + sizeOther.x &&
            posSelf.x + sizeSelf.x > posOther.x &&
            posSelf.y < posOther.y + sizeOther.y &&
            posSelf.y + sizeSelf.y > posOther.y);

}

int CheckCollisionVec2Mat(Vector2 posSelf, Vector2 sizeSelf, int matPosI, int matPosJ){
    Vector2 matPosVec2=MatrixPosToVec2(matPosI, matPosJ);
    return CheckCollisionVec2Vec2(posSelf,sizeSelf,matPosVec2,(Vector2){SQR_SIZ, SQR_SIZ});
}

int CheckFutureCollisionVec2Vec2(Vector2 posSelf, Vector2 sizeSelf, Vector2 velocitySelf, Vector2 posOther, Vector2 velocityOther, Vector2 sizeOther){
    Vector2 futureSelfPos={posSelf.x+velocitySelf.x, posSelf.y+velocitySelf.y};
    Vector2 futureOtherPos={posOther.x+velocityOther.x, posOther.y+velocityOther.y};
    return CheckCollisionVec2Vec2(futureSelfPos, sizeSelf, futureOtherPos, sizeOther);
}

int CheckFutureCollisionVec2Mat(Vector2 posSelf, Vector2 sizeSelf, Vector2 velocitySelf, int matPosI, int matPosJ){
    Vector2 futureSelfPos={posSelf.x+velocitySelf.x, posSelf.y+velocitySelf.y};
    return CheckCollisionVec2Mat(futureSelfPos, sizeSelf, matPosI, matPosJ);
}

int PlayerIsOnGround(Player p, int mat[MAT_HEIGHT][MAT_WIDTH]){
    int i=ScreenYToMatrixLine(p.position.y);
    int j=ScreenXToMatrixColumn(p.position.x);
    
    int cond1 = (i >= 0 && i < MAT_HEIGHT && j >= 0 && j < MAT_WIDTH) && (CheckCollisionVec2Mat(p.position,p.size,i,j) && mat[i][j]==1);
    int cond2 = (i+1 >= 0 && i+1 < MAT_HEIGHT && j >= 0 && j < MAT_WIDTH) && (CheckCollisionVec2Mat(p.position,p.size,i+1,j) && mat[i+1][j]==1);
    int cond3 = (i >= 0 && i < MAT_HEIGHT && j+1 >= 0 && j+1 < MAT_WIDTH) && (CheckCollisionVec2Mat(p.position,p.size,i,j+1) && mat[i][j+1]==1);
    int cond4 = (i+1 >= 0 && i+1 < MAT_HEIGHT && j+1 >= 0 && j+1 < MAT_WIDTH) && (CheckCollisionVec2Mat(p.position,p.size,i+1,j+1) && mat[i+1][j+1]==1);
    
    return (cond1 || cond2 || cond3 || cond4);
}

int PlayerWillBeOnGround(Player *p, int mat[MAT_HEIGHT][MAT_WIDTH]){
    Player futurePlayer=*p;
    futurePlayer.position.x+=futurePlayer.velocity.x;
    futurePlayer.position.y+=futurePlayer.velocity.y;

    if(PlayerIsOnGround(futurePlayer, mat)){
        p->velocity.y=0;
        p->velocity.x=0;
        int i = ScreenYToMatrixLine(futurePlayer.position.y);
        int j = ScreenXToMatrixColumn(futurePlayer.position.x);

        if(p->position.y<futurePlayer.position.y){

            p->position=(Vector2){futurePlayer.position.x, MatrixPosToVec2(i+1,j).y};
        }
        else{

            p->position=(Vector2){futurePlayer.position.x, MatrixPosToVec2(i-1,j).y};
        }
        if (PlayerIsOnGround(p, mat)){
            if(p->position.x<futurePlayer.position.x){

                p->position.x=MatrixPosToVec2(i,j+1).x;
            }
            else{

                p->position.x=MatrixPosToVec2(i,j-1).x;
            }
        }
        return 1;
    }
    return 0;
}

int GroundBelow(Vector2 pos, Vector2 size, int mat[MAT_HEIGHT][MAT_WIDTH]){
    Player fakePlayer={0}; //acabei nn fazendo a função de forma genérica o suficiente, ignorar
    fakePlayer.position=pos+SQR_SIZ/2; //o suficiente pra entrar no bloco
    fakePlayer.size=size;

    return PlayerIsOnGround(fakePlayer, mat);
}

int WillThereBeGroundBelow(Vector2 pos, Vector2 size, Vector2 velocity, int mat[MAT_HEIGHT][MAT_WIDTH]){
    return GroundBelow((Vector2){pos.x+velocity.x,pos.y+velocity.y},size,mat);
}

int PlayerEnemyCollision(Player p, EnemyManager e){
    
    for(int i=0; i<e.quantEnemies; i++){
        if(CheckCollisionVec2Vec2(p.position, p.size, e.enemies[i].position, e.enemies[i].size)){
            return 1;
        }
    }
    return 0;
}

int PlayerEnemyFutureCollision(Player p, EnemyManager e){
    
    for(int i=0; i<e.quantEnemies; i++){
        if(CheckFutureCollisionVec2Vec2(p.position, p.size, p.velocity, e.enemies[i].position, e.enemies[i].size, e.enemies[i].velocity)){
            return 1;
        }
    }
    return 0;
}

int PlayerStompEnemy(Player p, EnemyManager e, int* index){
    *index=-1;
    for(int i=0;i<e.quantEnemies;i++){
        if(CheckFutureCollisionVec2Vec2(p.position, p.size, p.velocity, e.enemies[i].position, e.enemies[i].size, e.enemies[i].velocity)){
            if((p.position.y+p.size.y<e.enemies[i].position.y)){
                *index=i;
                return 1;
            }
        }
    }
    return 0;
}

int OutOfBoundsCheck(Vector2 pos, Vector2 size){
    return !CheckCollisionVec2Vec2(pos, size, (Vector2){0,0},(Vector2){(float)SCR_WIDTH,(float)SCR_HEIGHT});
}

int PlayerIsOnStair(Player *p, int mat[MAT_HEIGHT][MAT_WIDTH]) {
    // Usamos um offset para garantir que a verificação teste os pixels internos da struct Player
    float offset = 1.0f;

    int p_l_head = mat[ScreenYToMatrixLine(p->position.y)][ScreenXToMatrixColumn(p->position.x)];
    int p_r_head = mat[ScreenYToMatrixLine(p->position.y)][ScreenXToMatrixColumn(p->position.x + p->size.x - offset)];

    int p_l_feet = mat[ScreenYToMatrixLine(p->position.y + p->size.y - offset)][ScreenXToMatrixColumn(p->position.x)];
    int p_r_feet = mat[ScreenYToMatrixLine(p->position.y + p->size.y - offset)][ScreenXToMatrixColumn(p->position.x + p->size.x - offset)];

    /*
        VERIFICAÇÕES
        Mapeamento conforme o draw-map.c:
        2 = 'H' / 'h' (Escada normal)
        3 = 'S' / 's' (Base da escada)
        4 = 'D' / 'd' (Topo da escada)
    */

    if ((p_l_head == 3 || p_r_head == 3)) {                                                    
        // Se a cabeça tiver no S (3), só pode subir
        p->acceleration.y = 0;
        p->velocity.y = 0;
        return 1;
    }
    else if ((p_l_feet == 4 || p_r_feet == 4) && (p_l_head == 4 || p_r_head == 4)) {   
        // Se o pé e a cabeça tiverem no D (4), só pode descer
        return 4;
    }
    else if ((p_l_feet == 2 || p_r_feet == 2) && (p_l_head == 4 || p_r_head == 4)) {  
        // Amortecimento no topo da escada na hora de descer
        p->acceleration.y = 0;
        p->velocity.y = 0;
        return 3;
    }
    else if ((p_l_feet == 2 || p_r_feet == 2) || (p_l_head == 2 || p_r_head == 2)) {   
        // Na escada normal, podemos subir ou descer
        p->acceleration.y = 0;
        p->velocity.y = 0;
        return 2;
    }
    else {
        return 0;
    }
}