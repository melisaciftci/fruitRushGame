#ifndef PROTOTYPES_H_
#define PROTOTYPES_H_

#include "defines.h"
#include "raylib.h"

void ReadHighScore();
void WriteHighScore();
void DrawStartScreen();
void StartGame();
void SwitchTiles();
bool MatchAndCreate();
void UpdateGame();
void SoundInput(Vector2 lastMousePosition);
void SoundInput2(Vector2 lastMousePosition);
void CheckSpecialTiles(Tile grid[GRID_ROWS][GRID_COLS]);
void CheckFallenTiles(Tile grid[GRID_ROWS][GRID_COLS]);
void DrawingGame(int currentLevel);
void DrawingBackground();
void DrawingGrid(Tile grid[GRID_ROWS][GRID_COLS]);
void DrawingLevels();
void CreateLevelObjectives(int currentLevel);
void CheckLevel1Objectives();
void CheckLevel2Objectives();
void CheckLevel3Objectives();
void CheckLevelObjectives();
void ShowWinMessage();
void ShowFailMessage();
void ShowHighScore();
void ShowGameEndMessage();
void CreateLevel(int currentLevel);

#endif