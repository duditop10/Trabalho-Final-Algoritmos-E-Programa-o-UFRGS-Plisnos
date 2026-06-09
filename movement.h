#pragma once

#include "structs.h"
#include "global-defs.h"

#define MAX_VELOCITY 4.0
#define ACCEL 0.5
#define DEACCEL 2.0
#define JUMP_VEL -4.0 
#define STAIR_VEL 4.0


void SetGravity(Player *, float);
void ApplyGravity(Player *);
void HorizontalMovement(Player *, int [MAT_HEIGHT][MAT_WIDTH]);
void Damping(Player *, int [MAT_HEIGHT][MAT_WIDTH]);
void Jump(Player *);
void OnStairMove(Player *, int);
void Move(Player *, int [MAT_HEIGHT][MAT_WIDTH]);