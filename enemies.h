#pragma once

#include "structs.h"
#include "global-defs.h"

/*
    FUNÇÃO PRINCIPAL QUE ATUALIZA TODOS OS INIMIGOS
*/
void UpdateEnemies(EnemyManager *, int [MAT_HEIGHT][MAT_WIDTH]);

/*
    FUNÇÃO PARA MATAR O INIMIGO (REMOVE DO VETOR E AJUSTA A QUANTIDADE)
*/
void KillEnemy(EnemyManager *, int);

/*
    FUNÇÃO AUXILIAR DE MOVIMENTAÇÃO DE UM ÚNICO INIMIGO
*/
void MoveEnemy(Enemy *, int [MAT_HEIGHT][MAT_WIDTH]);