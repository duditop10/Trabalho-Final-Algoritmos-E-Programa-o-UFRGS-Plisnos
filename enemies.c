#include "enemies.h"
#include "collisions.h"
#include "raylib.h"
#include <stdlib.h>

#include "enemies.h"
#include "collisions.h"
#include "raylib.h"
#include <stdlib.h>

void KillEnemy(EnemyManager *e, int index) {
    if (index < 0 || index >= e->quantEnemies) return;
    
    // Troca o inimigo morto pelo último da lista (swap)
    e->enemies[index] = e->enemies[e->quantEnemies - 1];
    e->quantEnemies--;
}

void MoveEnemy(Enemy *e, int mat[MAT_HEIGHT][MAT_WIDTH]) {

    if (e->isClimbing) {
        e->position.y += e->velocity.y;

        int i = ScreenYToMatrixLine(e->position.y + e->size.y / 2);
        int j = ScreenXToMatrixColumn(e->position.x + e->size.x / 2);
        int currentTile = GetMatrixValueSafe(mat, i, j);

        if ((e->velocity.y < 0 && currentTile == 4) || // Chegou no topo
            (e->velocity.y > 0 && currentTile == 3)) {  // Chegou na base
            
            e->velocity.y = 0;
            e->velocity.x = 2.0f; 
            int i = ScreenYToMatrixLine(e->position.y + e->size.y / 2);

            Vector2 posSnap = MatrixPosToVec2(i + 1, 0); 
            e->position.y = posSnap.y - e->size.y; 

            e->isClimbing = 0;

        }
        return; 
    }


    int i = ScreenYToMatrixLine(e->position.y + e->size.y / 2);
    int j = ScreenXToMatrixColumn(e->position.x + e->size.x / 2);
    int currentTile = GetMatrixValueSafe(mat, i, j);

    if (e->canClimbLadders) {
        if (currentTile == 3 && GetRandomValue(0, 100) < 50) { // Chance reduzida para teste
            e->velocity.y = -2.0f;
            e->velocity.x = 0;
            e->isClimbing = 1;
            return;
        } else if (currentTile == 4 && GetRandomValue(0, 100) < 50) {
            e->velocity.y = 2.0f;
            e->velocity.x = 0;
            e->isClimbing = 1;
            return;
        }
    }

    if (e->velocity.y == 0) e->velocity.y += 0.2f;

    if (e->velocity.x == 0) e->velocity.x = 2.0f;
    if (!WillThereBeGroundBelow(e->position, e->size, (Vector2){e->velocity.x * 10, 0}, mat)) {
        e->velocity.x *= -1;
    }
    e->position.x += e->velocity.x;

    if (PlayerIsOnGround((Player){e->position, {0}, {0}, e->size, 0}, mat)) {
        e->velocity.y = 0;
    }
}

void UpdateEnemies(EnemyManager *e, int mat[MAT_HEIGHT][MAT_WIDTH]) {
    for (int i = 0; i < e->quantEnemies; i++) {
        MoveEnemy(&e->enemies[i], mat);
    }
}