#include "raylib.h"
#include "structs.h"
#include <string.h>

#define MAX_VELOCITY 3.7
#define MAT_WIDTH 30
#define MAT_HEIGHT 30

const int screenWidth = 600;
const int screenHeight = 800;

char mat[MAT_HEIGHT][MAT_WIDTH];

void DrawPlayer(Player player);
int OutOfBoundsCheck(Player* p);
void MovementInput(Player* p);
int GroundCollisionCheck(Player* p);
Vec2 MatrixPosToVec2(int i, int j);
void DrawMatrix();
int IsGroundWithinJumpRange(Player* p);

int main(){
    memcpy(mat[20],"  ZZZZZZZZZZZZZZZZZZZZZZZZ    ",30);
    Player p = {
        .position={300,400},
        .velocity={0,0},
        .acceleration={0,0},
        .size={screenWidth/MAT_HEIGHT,screenWidth/MAT_WIDTH}
    };


    InitWindow(screenWidth, screenHeight, "Donkey Kong");

    SetTargetFPS(60);               

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        p.acceleration.y=9.8/60;

        p.velocity.x+=p.acceleration.x;
        p.velocity.y+=p.acceleration.y;


        MovementInput(&p);

        p.position.x+=p.velocity.x;
        p.position.y+=p.velocity.y;

        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawMatrix();
            DrawPlayer(p);

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
    int downBoundCheck= (*p).position.y + (*p).size.y + (*p).velocity.y > screenHeight;

    if(upBoundCheck){
        (*p).velocity.y = 0;
        (*p).position.y = 0;
        outOfBounds=1;
    }
    else if(downBoundCheck){
        (*p).velocity.y = 0;
        (*p).position.y = screenHeight - (*p).size.y;
        outOfBounds=2;
    }

    int leftBoundCheck = (*p).position.x + (*p).velocity.x < 0;
    int rightBoundCheck = (*p).position.x + (*p).size.x + (*p).velocity.x > screenWidth;
    
    if(leftBoundCheck){
        (*p).velocity.x = 0;
        (*p).position.x = 0;
        if(!outOfBounds){
            outOfBounds=1;
        }
    }
    else if(rightBoundCheck){
        (*p).velocity.x = 0;
        (*p).position.x = screenWidth - (*p).size.x;
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

int GroundCollisionCheck(Player* p){
    int i = (int)(((*p).position.y-screenHeight+screenWidth)/(screenWidth/MAT_WIDTH));
    int j = (int)((*p).position.x/(screenWidth/MAT_WIDTH));
    if(mat[i+1][j]=='Z'){
        if (MatrixPosToVec2(i+1,j).y<(*p).position.y+(*p).size.y+(*p).velocity.y){
            (*p).velocity.y=0;
            (*p).position.y=MatrixPosToVec2(i+1,j).y-(*p).size.y;
            return 1;
        }
        if (MatrixPosToVec2(i+1,j).y<(*p).position.y+(*p).size.y){
            return 1;
        }
        
    }
    j = (int)(((*p).position.x+(*p).size.x)/(screenWidth/MAT_WIDTH));
    if(mat[i+1][j]=='Z'){
        if (MatrixPosToVec2(i+1,j).y<(*p).position.y+(*p).size.y+(*p).velocity.y){
            (*p).velocity.y=0;
            (*p).position.y=MatrixPosToVec2(i+1,j).y-(*p).size.y;
            return 1;
        }
        if (MatrixPosToVec2(i+1,j).y<(*p).position.y+(*p).size.y){
            return 1;
        }
        
    }
    if (OutOfBoundsCheck(p)==2){
        return 1;
    }
    return 0;
    // the code below is only there as am example, it's commented out because it's included in the other situations
    // if(mat[i][j]=='Z'){ //this means you're inside the platform, specifically, this means your y value is 
                        // either at or below the ground's
        // return 0;//allows you to pass through from under the platform;

    // }
}

void DrawMatrix(){
    for (int i=0; i<MAT_HEIGHT; i++){
        for (int j=0; j<MAT_WIDTH; j++){
            if (mat[i][j]=='Z'){
                Vec2 pos=MatrixPosToVec2(i,j);
                DrawRectangle(pos.x,pos.y, screenWidth/MAT_HEIGHT, screenWidth/MAT_WIDTH, BLUE);
            }
        }
    }
}

int IsGroundWithinJumpRange(Player* p) {
    // Approximate max jump height based on jump velocity 4 and gravity 0.163
    const float MAX_JUMP_DIST = 52.0f; 
    
    int tileSize = screenWidth / MAT_WIDTH;    // 20 pixels
    int yOffset = screenHeight - screenWidth;  // 200 pixel offset where matrix starts

    // 1. Determine the horizontal grid range (columns) the player covers
    int startJ = (int)(((*p).position.x+(*p).velocity.x) / tileSize);
    int endJ = (int)(((*p).position.x+(*p).size.x+(*p).velocity.x) / tileSize);

    // 2. Determine the vertical grid range (rows) to search
    // We start searching from the row immediately at/below the player's feet
    float playerBottom = p->position.y + p->size.y;
    int startI = (int)((playerBottom - yOffset) / tileSize);
    
    // We stop searching at the row corresponding to feet + jump height
    int endI = (int)((playerBottom + MAX_JUMP_DIST - yOffset) / tileSize);

    // 3. Scan the matrix within these bounds
    for (int i = startI; i <= endI; i++) {
        // Stay within matrix bounds vertically
        if (i < 0 || i >= MAT_HEIGHT) continue;

        if (mat[i][startJ] == 'Z') {
            if(mat[i][endJ] =='Z'){
                return 1; // Ground found within range
            }
        }
        
    }

    // 4. Check for the screen bottom (as handled in OutOfBoundsCheck)
    if (playerBottom + MAX_JUMP_DIST >= screenHeight) {
        return 1;
    }

    return 0; // No ground found within jump distance
}

Vec2 MatrixPosToVec2(int i, int j){
    int yOffset=screenHeight-screenWidth; // starts at this point to make the window essentially square
    int size = screenWidth/MAT_WIDTH;

    Vec2 result;
    result.x=(float)j*size;
    result.y=(float)i*size+yOffset;
    return result;
}

void DrawPlayer(Player player){
    DrawRectangle(
        (int)(player.position.x),
        (int)(player.position.y),
        (int)player.size.x,
        (int)player.size.y, 
        RED
    );
}