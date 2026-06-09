#pragma once

#include "raylib.h"
#include "global-defs.h"



typedef struct Player{
    Vector2 position; //posição x e y (em pixel, x=0 e y=0 é o canto superior esquerdo)
    Vector2 velocity; //componente x é velocidade horizontal, y é vertical (em pixels/frame)
    Vector2 acceleration; //componente x é aceleração horizontal, y é vertical (em pixels/frame²)
    Vector2 size; //usado para calculos de colisão (e desenho)
    int lives;
} Player;

typedef struct Enemy{
    int alive;
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 size;
    int canClimbLadders;
    int isClimbing;
} Enemy;

typedef struct EnemyManager{
    Enemy enemies[MAT_HEIGHT*MAT_WIDTH+1];
    int quantEnemies;
} EnemyManager;