#pragma once

#include "raylib.h"
#include "global-defs.h"
#include "structs.h"

int ScreenYToMatrixLine(float y);
int ScreenXToMatrixColumn(float x);
Vector2 MatrixPosToVec2(int i, int j);


int CheckCollisionVec2Vec2(Vector2 posSelf, Vector2 sizeSelf, Vector2 posOther, Vector2 sizeOther);
int CheckCollisionVec2Mat(Vector2 posSelf, Vector2 sizeSelf, int matPosI, int matPosJ);

Rectangle GetPlayerHitbox(Player);

int CheckFutureCollisionVec2Vec2(Vector2 posSelf, Vector2 sizeSelf, Vector2 velocitySelf, Vector2 posOther, Vector2 velocityOther, Vector2 sizeOther);
int CheckFutureCollisionVec2Mat(Vector2 posSelf, Vector2 sizeSelf, Vector2 velocitySelf, int matPosI, int matPosJ);

int PlayerIsOnGround(Player p, int mat[MAT_HEIGHT][MAT_WIDTH]);
int PlayerWillBeOnGround(Player p, int mat[MAT_HEIGHT][MAT_WIDTH]);

int GetMatrixValueSafe(int [MAT_HEIGHT][MAT_WIDTH], int, int);


int GroundBelow(Vector2 pos, Vector2 size, int mat[MAT_HEIGHT][MAT_WIDTH]);
int WillThereBeGroundBelow(Vector2 pos, Vector2 size, Vector2 velocity, int mat[MAT_HEIGHT][MAT_WIDTH]);

int OutOfBoundsCheck(Vector2 pos, Vector2 size);
int PlayerIsOnStair(Player *, int [MAT_HEIGHT][MAT_WIDTH]);