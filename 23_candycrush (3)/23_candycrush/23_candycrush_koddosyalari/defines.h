#ifndef DEFINES_H_
#define DEFINES_H_

#include "raylib.h"
#include "time.h"
#include <string.h> // strcpy fonksiyonunu kullanmak için

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define GRID_ROWS 9
#define GRID_COLS 16
#define TILE_SIZE 80
#define NUM_COLORS 4
#define NUM_SPECIAL_COLORS 1 // Siyah kare için

#define MATCH_SCORE 10 // Her eşleşme için verilecek puan miktarı

#define MAX_MOVES 50 // Maksimum hamle sayısı

#define BACKGROUND_GRID_WIDTH  1280 // Çerçeve tasarımının genişliği
#define BACKGROUND_GRID_HEIGHT 720  // Çerçeve tasarımının yüksekliği
#define BACKGROUND_TEXTURE_FILENAME "assets/background.png"
#define BACKGROUND_GRID_TEXTURE_FILENAME "assets/backgroundgrid.png"
#define START_BACKGROUND_TEXTURE_FILENAME "assets/startbackground.png"
#define RED_TEXTURE_FILENAME "assets/red.png"
#define GREEN_TEXTURE_FILENAME "assets/green.png"
#define BLUE_TEXTURE_FILENAME "assets/purple.png"
#define YELLOW_TEXTURE_FILENAME "assets/orange.png"
#define COOKIE_TEXTURE_FILENAME "assets/cookie.png"
#define BLACK_TILE_INDEX NUM_COLORS // Siyah rengi temsil etmek için (Cookie)
#define START_SCREEN_TEXTURE_FILENAME "assets/startscreen.png"
#define YOULOST_SCREEN_TEXTURE_FILENAME "assets/youlostscreen.png"
#define YOUWIN_SCREEN_TEXTURE_FILENAME "assets/youwinscreen.png"
#define GAMEOVER_SCREEN_TEXTURE_FILENAME "assets/gameoverscreen.png"
#define BROKETHEHIGH_SCREEN_TEXTURE_FILENAME "assets/brokethehighscreen.png"


Texture2D backgroundTexture; // Arka plan görüntüsünü saklayacak değişken
Texture2D backgroundGridTexture; // Arka plan ızgarası görüntüsünü saklayacak değişken
Texture2D startBackgroundTexture;
Texture2D levelBackgroundTexture;
Texture2D level2BackgroundTexture;
Texture2D failBackgroundTexture;
Texture2D gameOverBackgroundTexture;
int backgroundGridPosX;
int backgroundGridPosY;
Sound fxBackground;
Sound fxTap;

bool level3TimerStart = false;
static bool isDragging = false; //Fare sürüklendi mi sürüklenmedi mi; false ise işlem tamamlandı; true ise işlem başladı
float level3Timer = 0;
// Başlangıç ekranında mı yoksa oyun ekranında mı olduğumuzu belirleyen bir durum değişkeni
bool startScreen = true;


// Her bir karenin bilgilerini saklayacak olan yapı
typedef struct
{
    int roww;
    int coll;
    int colorIndex;
    bool selected;
    Texture2D texture;
    bool cookieTile; 
    time_t creationTime; // Zaman bilgisini saklamak için
} Tile;

// Grid'i tanımla
Tile grid[GRID_ROWS][GRID_COLS];

//Renkler ve dosya adları eşleştirildi
const char* filenames[NUM_COLORS]={BLUE_TEXTURE_FILENAME, RED_TEXTURE_FILENAME, GREEN_TEXTURE_FILENAME, YELLOW_TEXTURE_FILENAME};


// Oyunun durum bilgileri
int score = 0;      // Oyuncunun toplam puanı
int movesLeft = MAX_MOVES; // Geriye kalan hamle sayısı
int cookieCounter = 0; // Siyah kare sayacı
int cookies = 0;        // Eşleşmeler sonucu siyah kare sayısını tutan değişken
int currentLevel=1;
int randomColorIndex;
int highScore = 0;
int offsetX;
int offsetY;

static Vector2 lastMousePosition = {-1, -1};
static int startingCol = -1, startingRow = -1;

static Tile draggingTile; // Sürüklenen şekerin geçici bilgileri
static Rectangle draggingRect; // Sürüklenen şekerin görsel dikdörtgeni

#endif