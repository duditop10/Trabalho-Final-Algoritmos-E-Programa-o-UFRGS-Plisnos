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

#define SAVE_FILE      "savegame.txt"
#define LEVELS_PER_ROW 4


typedef enum GameState{
    MENU,
    LEVEL_SELECT,
    GAME,
    PAUSE,
    GAME_OVER,
    GAME_WIN,
    GAME_LOAD,
    GAME_FINISH
}GameState;

void LevelStart (char[], int[MAT_HEIGHT][MAT_WIDTH], EnemyManager *, Player *);
int  LevelUpdate(time_t, time_t *, int[MAT_HEIGHT][MAT_WIDTH], EnemyManager *, Player *);
void LevelEnd   (Player *);
void WriteSaveData(void);
void LoadSaveData (void);

typedef struct SaveData {
    int          lastLevelPlayed;
    int          highestUnlocked;
    unsigned int highScores[MAX_NUM_LEVELS + 1];
} SaveData;

GameState    game            = MENU;
int          menuOption      = 0;
int          lastLevelPlayed = 0;
int          newHighScore    = 0;   // set when player beats their best on a level
unsigned int prevHighScore   = 0;   // old best before current run updated it
SaveData     saveData;
 


unsigned int max(unsigned int a, unsigned int b){
    if(a>=b){
        return a;
    }
    return b;
}


static void DrawCentered(Font font, const char *text, float y,
                         float size, float sp, Color col) {
    Vector2 m = MeasureTextEx(font, text, size, sp);
    DrawTextEx(font, text, (Vector2){ SCR_WIDTH / 2.0f - m.x / 2.0f, y },
               size, sp, col);
}
 
// ─── Save / Load ─────────────────────────────────────────────────────────────
void WriteSaveData(void) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) { printf("Cannot write save file.\n"); return; }
    fprintf(f, "%d\n%d\n", saveData.lastLevelPlayed, saveData.highestUnlocked);
    for (int i = 0; i <= MAX_NUM_LEVELS; i++)
        fprintf(f, "%u\n", saveData.highScores[i]);
    fclose(f);
}
void LoadSaveData(void) {

    saveData.lastLevelPlayed = 0;
    saveData.highestUnlocked = 0;
    for (int i = 0; i <= MAX_NUM_LEVELS; i++) saveData.highScores[i] = 0;
 
    FILE *f = fopen(SAVE_FILE, "r");
    if (!f) { WriteSaveData(); return; }   // Se não tem arquivo, cria arquivo
    fscanf(f, "%d\n%d\n", &saveData.lastLevelPlayed, &saveData.highestUnlocked);
    for (int i = 0; i <= MAX_NUM_LEVELS; i++)
        fscanf(f, "%u\n", &saveData.highScores[i]);
    fclose(f);
 
    // evitar dados corrompidos
    if (saveData.lastLevelPlayed < 0)               saveData.lastLevelPlayed = 0;
    if (saveData.lastLevelPlayed > MAX_NUM_LEVELS)  saveData.lastLevelPlayed = MAX_NUM_LEVELS;
    if (saveData.highestUnlocked < 0)               saveData.highestUnlocked = 0;
    if (saveData.highestUnlocked > MAX_NUM_LEVELS)  saveData.highestUnlocked = MAX_NUM_LEVELS;
    if (saveData.lastLevelPlayed > saveData.highestUnlocked)
        saveData.lastLevelPlayed = saveData.highestUnlocked;
}



int main(){
    int mat[MAT_HEIGHT][MAT_WIDTH];
    EnemyManager enemyMan;
    Player p;
    char filename[64];
 
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Donkey Kong");
    SetTargetFPS(60);
 
    LoadSaveData();
    lastLevelPlayed = saveData.lastLevelPlayed;
    snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
           
    
    time_t initTime=time(NULL);
    time_t timeElapsed=0;
    time_t pausedTime=0;
    int shouldQuit=0;
    
    while (!WindowShouldClose()&&!shouldQuit)    // Detect window close button or ESC key
    {   
        
        

        switch (game) {
            case MENU: {
                int mv = IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP);
                menuOption = (menuOption + mv + 3) % 3; // 3 opções: 0=Continue 1=LevelSelect 2=Quit
    
                if (IsKeyPressed(KEY_ENTER)) {
                    if (menuOption == 0) {              
                        lastLevelPlayed = saveData.lastLevelPlayed;
                        snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
                        game = GAME_LOAD; menuOption = 0;
                    } else if (menuOption == 1) {       
                        menuOption = saveData.lastLevelPlayed;
                        game = LEVEL_SELECT;
                    } else {                            
                        shouldQuit = 1;
                    }
                }
    
                Font  f  = GetFontDefault();
                float sp = 2.0f;
                BeginDrawing();
                    ClearBackground((Color){20, 20, 40, 255});
                    DrawCentered(f, "MARIO    GAMES", 90, 56, sp, YELLOW);
    
                    const char *opts[3] = {"Continue", "Level Select", "Quit"};
                    for (int i = 0; i < 3; i++) {
                        float y   = SCR_HEIGHT / 2.0f - 20 + i * 65;
                        Color col = (i == menuOption) ? WHITE : GRAY;
                        if (i == menuOption) {
                            Vector2 m = MeasureTextEx(f, opts[i], 34, sp);
                            DrawTextEx(f, ">",
                                    (Vector2){ SCR_WIDTH / 2.0f - m.x / 2.0f - 30, y },
                                    34, sp, YELLOW);
                        }
                        DrawCentered(f, opts[i], y, 34, sp, col);
                    }
                    DrawCentered(f, TextFormat("Last level played: %d", saveData.lastLevelPlayed),
                                SCR_HEIGHT - 38, 18, sp, DARKGRAY);
                EndDrawing();
            } break;
    
            
            case LEVEL_SELECT: {
                int maxSel = saveData.highestUnlocked;
    
                
                if (IsKeyPressed(KEY_RIGHT)) menuOption++;
                if (IsKeyPressed(KEY_LEFT))  menuOption--;
                if (IsKeyPressed(KEY_DOWN))  menuOption += LEVELS_PER_ROW;
                if (IsKeyPressed(KEY_UP))    menuOption -= LEVELS_PER_ROW;
                if (menuOption < 0)          menuOption = maxSel;
                if (menuOption > maxSel)     menuOption = 0;
    
                if (IsKeyPressed(KEY_ESCAPE)) { game = MENU; menuOption = 0; }
    
                if (IsKeyPressed(KEY_ENTER)) {
                    lastLevelPlayed = menuOption;
                    snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
                    game = GAME_LOAD; menuOption = 0;
                }
    
                Font  f  = GetFontDefault();
                float sp = 2.0f;
    
                // Grid
                int bW = 100, bH = 80, gap = 14;
                int totalW  = LEVELS_PER_ROW * bW + (LEVELS_PER_ROW - 1) * gap;
                int originX = SCR_WIDTH / 2 - totalW / 2;
                int originY = 105;
    
                BeginDrawing();
                    ClearBackground((Color){20, 20, 40, 255});
                    DrawCentered(f, "LEVEL  SELECT", 28, 42, sp, YELLOW);
                    DrawCentered(f, "Arrows to navigate    ENTER to play    ESC back",
                                SCR_HEIGHT - 30, 15, sp, DARKGRAY);
    
                    for (int lvl = 0; lvl <= MAX_NUM_LEVELS; lvl++) {
                        int row = lvl / LEVELS_PER_ROW;
                        int col = lvl % LEVELS_PER_ROW;
                        int bx  = originX + col * (bW + gap);
                        int by  = originY + row * (bH + gap + 22);
    
                        int unlocked = (lvl <= saveData.highestUnlocked);
                        int selected = (lvl == menuOption);
    
                        // Box colours
                        Color bg  = unlocked ? (selected ? (Color){70, 80, 140, 255}
                                                        : (Color){50, 55,  90, 255})
                                            : (Color){35, 35, 45, 255};
                        Color brd = selected  ? YELLOW
                                : unlocked  ? (Color){100, 110, 180, 255}
                                            : DARKGRAY;
                        DrawRectangle    (bx, by, bW, bH, bg);
                        DrawRectangleLines(bx, by, bW, bH, brd);
    
                        // Número do level
                        const char *numTxt = TextFormat("%d", lvl);
                        Vector2 nm = MeasureTextEx(f, numTxt, 26, sp);
                        DrawTextEx(f, numTxt,
                                (Vector2){ bx + bW / 2.0f - nm.x / 2.0f, by + 10 },
                                26, sp, unlocked ? WHITE : DARKGRAY);
    
                        // High score ou LOCKED
                        if (unlocked) {
                            const char *hs = TextFormat("%u", saveData.highScores[lvl]);
                            Vector2 hm = MeasureTextEx(f, hs, 14, sp);
                            DrawTextEx(f, hs,
                                    (Vector2){ bx + bW / 2.0f - hm.x / 2.0f, by + 48 },
                                    14, sp, GOLD);
                        } else {
                            const char *lk = "LOCKED";
                            Vector2 lm = MeasureTextEx(f, lk, 13, sp);
                            DrawTextEx(f, lk,
                                    (Vector2){ bx + bW / 2.0f - lm.x / 2.0f, by + 50 },
                                    13, sp, DARKGRAY);
                        }
                    }
                EndDrawing();
            } break;
    
            
            case GAME_LOAD:
                LevelStart(filename, mat, &enemyMan, &p);
                initTime    = time(NULL);
                timeElapsed = 0;
                game        = GAME;
                saveData.lastLevelPlayed=lastLevelPlayed;
                WriteSaveData();
                break;
    
            
            case GAME:
                LevelUpdate(initTime, &timeElapsed, mat, &enemyMan, &p);
                break;
    
            
            case PAUSE: {
                if (IsKeyPressed(KEY_P)) game = GAME;
                Font  f  = GetFontDefault();
                float sp = 2.0f;
                BeginDrawing();
                    DrawCentered(f, "PAUSED",            SCR_HEIGHT / 2.0f - 36, 50, sp, BLACK);
                    DrawCentered(f, "Press P to resume", SCR_HEIGHT / 2.0f + 30, 22, sp, DARKGRAY);
                EndDrawing();
            } break;
    
            
            case GAME_OVER: {
                int mv = IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP);
                menuOption = (menuOption + mv + 4) % 4;
    
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (menuOption) {
                    case 0: 
                        snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
                        game = GAME_LOAD; menuOption = 0; break;
                    case 1: game = LEVEL_SELECT; menuOption = 0; break;
                    case 2: game = MENU;         menuOption = 0; break;
                    case 3: shouldQuit = 1;      break;
                    }
                }
    
                Font  f  = GetFontDefault();
                float sp = 2.0f;
                BeginDrawing();
                    ClearBackground((Color){180, 20, 20, 255});
                    DrawCentered(f, "GAME OVER", 50, 52, sp, BLACK);
    
                    const char *opts[4] = {"Restart Level", "Level Select", "Menu", "Quit"};
                    float baseY = 3.0f * SCR_HEIGHT / 5.0f;
                    for (int i = 0; i < 4; i++) {
                        float y   = baseY + i * 55;
                        Color col = (i == menuOption) ? WHITE : GRAY;
                        if (i == menuOption) {
                            Vector2 m = MeasureTextEx(f, opts[i], 32, sp);
                            DrawTextEx(f, ">",
                                    (Vector2){ SCR_WIDTH / 2.0f - m.x / 2.0f - 30, y },
                                    32, sp, YELLOW);
                        }
                        DrawCentered(f, opts[i], y, 32, sp, col);
                    }
                EndDrawing();
            } break;
    
            case GAME_WIN: {
                int mv = IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP);
                menuOption = (menuOption + mv + 4) % 4;
    
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (menuOption) {
                    case 0: // Next Level ou Fim
                        if (lastLevelPlayed < MAX_NUM_LEVELS) {
                            lastLevelPlayed++;
                            snprintf(filename, sizeof(filename), "mapa%d.txt", lastLevelPlayed);
                            game = GAME_LOAD;
                        } else {
                            // Drain key queue so GAME_FINISH doesn't skip immediately
                            while (GetKeyPressed() != 0) {}
                            game = GAME_FINISH;
                        }
                        menuOption = 0; break;
                    case 1: game = LEVEL_SELECT; menuOption = 0; break;
                    case 2: game = MENU;         menuOption = 0; break;
                    case 3: shouldQuit = 1;      break;
                    }
                }
    
                Font  f  = GetFontDefault();
                float sp = 2.0f;
    
                long mins = timeElapsed / 60, secs = timeElapsed % 60;
                char timeBuf[16];
                snprintf(timeBuf, sizeof(timeBuf), "%02ld:%02ld", mins, secs);
    
                BeginDrawing();
                    ClearBackground((Color){20, 140, 30, 255});
                    DrawCentered(f, "YOU  WON!", 40, 52, sp, BLACK);
    
                    float iy = 120, isz = 22;
                    DrawCentered(f, TextFormat("Score:         %u", p.enemyScore),
                                iy, isz, sp, BLACK); iy += 38;
                    DrawCentered(f, TextFormat("Lives (%d) Score:   %u", p.lives, p.livesScore),
                                iy, isz, sp, BLACK); iy += 38;
                    DrawCentered(f, TextFormat("Time  (%s) Score:  %u", timeBuf, p.timeScore),
                                iy, isz, sp, BLACK); iy += 38;
                    DrawCentered(f, TextFormat("Total Score:         %u", p.score),
                                iy, isz, sp, BLACK); iy += 50;
    
                
                    if (newHighScore)
                        DrawCentered(f, TextFormat("*** NEW HIGH SCORE!  (prev: %u) ***", prevHighScore),
                                    iy, 20, sp, YELLOW);
                    else
                        DrawCentered(f, TextFormat("High Score: %u", saveData.highScores[lastLevelPlayed]),
                                    iy, 20, sp, GOLD);
    
                    const char *nextLbl = (lastLevelPlayed >= MAX_NUM_LEVELS) ? "Finish Game" : "Next Level";
                    const char *opts[4] = {nextLbl, "Level Select", "Menu", "Quit"};
                    float baseY = 3.0f * SCR_HEIGHT / 5.0f;
                    for (int i = 0; i < 4; i++) {
                        float y   = baseY + i * 52;
                        Color col = (i == menuOption) ? WHITE : GRAY;
                        if (i == menuOption) {
                            Vector2 m = MeasureTextEx(f, opts[i], 30, sp);
                            DrawTextEx(f, ">",
                                    (Vector2){ SCR_WIDTH / 2.0f - m.x / 2.0f - 30, y },
                                    30, sp, YELLOW);
                        }
                        DrawCentered(f, opts[i], y, 30, sp, col);
                    }
                EndDrawing();
            } break;

            case GAME_FINISH: {
                if (GetKeyPressed() != 0) {
                    // Zera o progresso mas mantém high scores e unlocks
                    saveData.lastLevelPlayed = 0;
                    lastLevelPlayed          = 0;
                    WriteSaveData();
                    snprintf(filename, sizeof(filename), "mapa%d.txt", 0);
                    game = MENU; menuOption = 0;
                }
    
                Font  f  = GetFontDefault();
                float sp = 2.0f;
                BeginDrawing();
                    ClearBackground((Color){15, 15, 50, 255});
                    DrawCentered(f, "Thanks for Playing!",             SCR_HEIGHT / 2.0f - 80, 46, sp, YELLOW);
                    DrawCentered(f, "You completed all levels!",        SCR_HEIGHT / 2.0f,       24, sp, WHITE);
                    DrawCentered(f, "Press any key to return to menu", SCR_HEIGHT / 2.0f + 55,  20, sp, GRAY);
                EndDrawing();
            } break;
 
        }
    }
 
    CloseWindow();
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
    char labelLives[] = "LIVES: ";
    const char* valueLives = TextFormat("%d", p->lives);
    
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

        prevHighScore = saveData.highScores[lastLevelPlayed];
        newHighScore = 0;
        
        if (p->score > prevHighScore) {
            saveData.highScores[lastLevelPlayed] = p->score;
            newHighScore = 1;
        }

        if (lastLevelPlayed == saveData.highestUnlocked && saveData.highestUnlocked < MAX_NUM_LEVELS) {
            saveData.highestUnlocked++;
        }
        
        saveData.lastLevelPlayed = lastLevelPlayed;
        WriteSaveData();

        return 0;
    }
    return 1;
} 

void LevelEnd(Player* p){
    p->livesScore=p->lives*BASE_LIVE_SCORE;

    p->score=p->timeScore+p->enemyScore+p->livesScore;
    
    printf("%u|%u|%u|%u\n", p->timeScore, p->enemyScore, p->livesScore, p->score);    
}
