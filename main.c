#include "raylib.h"
#include "structs.h"
#include <string.h>
#include "global-defs.h"
#include "draw-map.h"
#include "movement.h"
#include "enemies.h"
#include "collisions.h"
#include <time.h> 
#include <stdio.h>
#include <string.h>

typedef enum GameState{
    MENU,
    LEVEL_SELECT,
    GAME,
    PAUSE,
    GAME_OVER,
    GAME_WIN,
    GAME_LOAD
}GameState;

void LevelStart(char filename[], int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager* enemyMan, Player* p);
int LevelUpdate(time_t initTime, time_t* timeElapsed, int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager* enemyMan, Player* p);
void LevelEnd(Player* p);

GameState game=GAME_LOAD;
int menuOption=0;
int lastLevelPlayed=0;

unsigned int max(unsigned int a, unsigned int b){
    if(a>=b){
        return a;
    }
    return b;
}

int main(){
    int mat[MAT_HEIGHT][MAT_WIDTH];
    EnemyManager enemyMan;
    Player p;


    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Donkey Kong");

    SetTargetFPS(60);               
    
    time_t initTime=time(NULL);
    time_t timeElapsed=0;
    time_t pausedTime=0;
    char filename[50];

    snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);

    int shouldQuit=0;
    while (!WindowShouldClose()&&!shouldQuit)    // Detect window close button or ESC key
    {   
        
        

        switch(game){

            case GAME_LOAD:
                LevelStart(filename, mat, &enemyMan, &p);
                initTime=time(NULL);
                timeElapsed=0;
                game=GAME;
            break;
            
            case GAME:
                LevelUpdate(initTime,&timeElapsed,mat,&enemyMan,&p);
            break;
            
            case GAME_OVER:
                BeginDrawing();
                    ClearBackground(RED);
                    float fontSize=40;
                    float spacing=2;
                    const char* gameOverLabel="Game Over";
                    Font font=GetFontDefault();
                    Vector2 textSize=MeasureTextEx(font, gameOverLabel, fontSize, spacing);
                    float posX = ((float)SCR_WIDTH/2.0) - (textSize.x/2.0);
                    float posY = ((float)SCR_HEIGHT/2.0) - (textSize.y/2.0);
                    DrawTextEx(font, gameOverLabel, (Vector2){posX, posY}, fontSize, spacing, WHITE);

                EndDrawing();
                
            break;
            case GAME_WIN:
                float fontSizeWin=40;
                float spacingWin=2;
                const char* gameWinLabel="You Won!";
                Font fontWin=GetFontDefault();
                Vector2 textSizeWin=MeasureTextEx(fontWin, gameWinLabel, fontSizeWin, spacingWin);
                float posXWin = ((float)SCR_WIDTH/2.0) - (textSizeWin.x/2.0);
                float posYWin = 40;
                Vector2 currentPos=(Vector2){posXWin, posYWin};
                
                int move=IsKeyPressed(KEY_DOWN)-IsKeyPressed(KEY_UP);

                menuOption+=move;
                menuOption=menuOption%4;

                if(menuOption<0){
                    menuOption+=3;
                }

                Color col1=BLACK;
                Color col2=BLACK;
                Color col3=BLACK;
                Color col4=BLACK;
                switch(menuOption){
                    case 0:
                        col1=WHITE;
                        if(IsKeyPressed(KEY_ENTER)){
                            if(lastLevelPlayed<MAX_NUM_LEVELS){
                                lastLevelPlayed++;
                            }
                            snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
                            game=GAME_LOAD;
                            menuOption=0;
                        }
                    break;

                    case 1:
                        col2=WHITE;
                        if(IsKeyPressed(KEY_ENTER)){
                            game=LEVEL_SELECT;
                            menuOption=0;
                        }
                    break;

                    case 2:
                        col3=WHITE;
                        if(IsKeyPressed(KEY_ENTER)){
                            game=MENU;
                            menuOption=0;
                        }
                    break;

                    case 3:
                        col4=WHITE;
                        if(IsKeyPressed(KEY_ENTER)){
                            shouldQuit=1;
                        }
                    break;

                    default:

                }
                
                
                
                BeginDrawing();

                    ClearBackground(GREEN);
                    

                    DrawTextEx(fontWin, gameWinLabel, currentPos, fontSizeWin, spacingWin, BLACK);
                    
                    fontSizeWin=20;

                    currentPos.y+=80;
                    const char* label=TextFormat("Score: %d", p.enemyScore);
                    Vector2 textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, BLACK);
                    
                    currentPos.y+=50;
                    label=TextFormat("Lives Left (%d): %d", p.lives, p.livesScore);
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, BLACK);

                    long minutes = timeElapsed / 60;
                    long seconds = timeElapsed % 60;
                    
                    char buffer[32];
                    snprintf(buffer, sizeof(buffer), "%02ld:%02ld", minutes, seconds);

                    currentPos.y+=50;
                    label=TextFormat("Total Time (%s): %d", buffer, p.timeScore);
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, BLACK);
                    
                    currentPos.y+=50;
                    label=TextFormat("Total Score: %d", p.score);
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, BLACK);

                    fontSizeWin=30;

                    currentPos.y=3*SCR_HEIGHT/5;
                    label="Next Level";
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, col1);

                    currentPos.y+=50;
                    label="Level Select";
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, col2);

                    currentPos.y+=50;
                    label="Menu";
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, col3);

                    currentPos.y+=50;
                    label="Quit";
                    textSiz=MeasureTextEx(fontWin, label, fontSizeWin, spacingWin);
                    currentPos.x = ((float)SCR_WIDTH/2.0) - (textSiz.x/2.0);
                    DrawTextEx(fontWin, label, currentPos, fontSizeWin, spacingWin, col4);
                EndDrawing();
            break;

            case PAUSE:
                if(IsKeyPressed(KEY_P)){
                    game=GAME;
                    
                }
                float fontSizePause=40;
                float spacingPause=2;
                const char* gamePauseLabel="Pause";
                Font fontPause=GetFontDefault();
                Vector2 textSizePause=MeasureTextEx(fontPause, gamePauseLabel, fontSizePause, spacingPause);
                float posXPause = ((float)SCR_WIDTH/2.0) - (textSizePause.x/2.0);
                float posYPause = ((float)SCR_HEIGHT/2.0) - (textSizePause.y/2.0);
                BeginDrawing();
                    
                    DrawTextEx(fontPause, gamePauseLabel, (Vector2){posXPause, posYPause}, fontSizePause, spacingPause, BLACK);

                EndDrawing();
                
            break;

            
        }
    }
    

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
 
void LevelStart(char filename[], int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager* enemyMan, Player* p){
    SetRandomSeed((unsigned int)time(NULL)); 
    memset(mat, 0, MAT_HEIGHT*MAT_WIDTH*sizeof(int));
    memset(p, 0, sizeof(*p));
    memset(enemyMan, 0, sizeof(*enemyMan));
    LoadMapToMatrix(filename, mat, enemyMan, p);
    LoadTextures();
    p->lives=LIVES;
}

int LevelUpdate(time_t initTime, time_t* timeElapsed, int mat[MAT_HEIGHT][MAT_WIDTH], EnemyManager* enemyMan, Player* p){
    
    if(WindowShouldClose()){
        return 0;
    }

    if(IsKeyPressed(KEY_P)){
        game=PAUSE;
        
    }

    char labelScore[] = "SCORE: ";
    const char* valueScore = TextFormat("%d", p->enemyScore);
    char labelLives[] = "HIT POINTS: ";
    const char* valueLives = TextFormat("%d0", p->lives);
    
    int fontSize=20;

    int labelWidthLives=MeasureText(labelLives,fontSize);
    int startXLives=20, startYLives=20;
    
    int labelWidthScore=MeasureText(labelScore,fontSize);
    int valueWidthScore=MeasureText(valueScore,fontSize);
    int startXScore=SCR_WIDTH-20-(labelWidthScore+valueWidthScore);
    int startYScore=20;
    
    if(p->invincibilityFrames>0){
        p->invincibilityFrames--;
    }
    SetGravity(p, 9.8/60);
    Move(p, mat, enemyMan);
    if(OutOfBoundsCheck(p->position, p->size)){
        p->lives=0;
    }
    UpdateEnemies(enemyMan, mat);
    printf("Lives: %d\n", p->lives);
    
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMap(*p, *enemyMan, mat);
        DrawText(labelScore, startXScore, startYScore, fontSize, RED);
        DrawText(valueScore, startXScore + labelWidthScore, startYScore,fontSize, GOLD);
        DrawText(labelLives, startXLives, startYLives, fontSize, RED);
        DrawText(valueLives, startXLives + labelWidthLives, startYLives, fontSize, GREEN);
    EndDrawing();
    if(p->lives<=0){
        game=GAME_OVER;
        LevelEnd(p);
        return 0;
    }
    if(PlayerIsOnFlag(*p, mat)){
        *timeElapsed=time(NULL)-initTime;
        p->timeScore=max(BASE_TIME_SCORE-(unsigned int)*timeElapsed,0) ;
        game=GAME_WIN;
        LevelEnd(p);
        return 0;
    }
    return 1;
} 

void LevelEnd(Player* p){
    p->livesScore=p->lives*BASE_LIVE_SCORE;

    p->score=p->timeScore+p->enemyScore+p->livesScore;
    
    printf("%u|%u|%u|%u\n", p->timeScore, p->enemyScore, p->livesScore, p->score);    
}
