#include <raylib.h>
#include <stdlib.h> 
#include <time.h>   // Level 3 zaman sayacı için ve cookie için 
#include "defines.h"
#include <stdio.h>
#include "prototypes.h"

// level 3'te Highscore kırıldığı zaman dosyaya yazdıran fonksiyon
void WriteHighScore() {
    FILE *file = fopen("highscore.txt", "w"); // Yazma modunda dosyayı aç

    if (file != NULL) { // Dosya başarılı bir şekilde açıldıysa
        fprintf(file, "%d", highScore); // Dosyaya highscore'u yaz
        fclose(file); // Dosyayı kapat
    } else {
        printf("Error: Could not open file for writing.\n");
    }
}

// level 3 için fonksiyon dosyadan highscore'u okuyor
void ReadHighScore() {
    FILE *file = fopen("highscore.txt", "r"); // Okuma modunda dosyayı aç

    if (file != NULL) { // Dosya başarılı bir şekilde açıldıysa
        fscanf(file, "%d", &highScore); // Dosyadan highscore'u oku
        fclose(file); // Dosyayı kapat
    } else {
        printf("Error: Could not open file for reading.\n");
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Candy Crush Saga");
    SetTargetFPS(60);

    InitAudioDevice();
    fxBackground = LoadSound("assets/Kids Melancholy 2.wav");
    PlaySound(fxBackground);

    score=0; //genel oyun başlangıcında oyun sıfırlanır

    // Çerçevemizin tasarımı
    backgroundGridTexture = LoadTexture(BACKGROUND_GRID_TEXTURE_FILENAME);

    // Çerçeve tasarımının ekranın tam ortasınaki konumu
    backgroundGridPosX = (GetScreenWidth() - BACKGROUND_GRID_WIDTH) / 2;
    backgroundGridPosY = (GetScreenHeight() - BACKGROUND_GRID_HEIGHT) / 2;

  //Hangi leveldeyiz onu tutuyor
    CreateLevel(currentLevel);

        // Başlangıç ekranında mıyız değil miyiz anlamak için
    bool startScreen = true;

    while (!WindowShouldClose() && movesLeft >= 0)
    {
        if (startScreen)
        {
            // Başlangıç ekranını çiziyoruz
            DrawStartScreen();

            // "P" tuşuna basıldığında pyun başlıyor ve 1. level açılıyor
            if (IsKeyPressed(KEY_P))
            {
                startScreen = false;
                StartGame();
            }
        }
        else
        {
            // Oyun ekranınını hangi leveldeysek ona göre güncelleyip çiziyoruz ses olacak mı olmayacak mı oyuncu karar veriyor
            UpdateGame();
            DrawingGame(currentLevel);
             if (IsKeyPressed(KEY_C)) {
                StopSound(fxBackground);
                CloseAudioDevice();
            }
            else if (IsKeyPressed(KEY_B)) {
                InitAudioDevice();
                PlaySound(fxBackground);
            }
        }

        // Arka plan sesi bitmişse yeniden oynatıyoruz böylece devamlı ses oluyor
        if (!IsSoundPlaying(fxBackground))
        {
            PlaySound(fxBackground);
        }
    }

    // Oyun kapandığında ses cihazını kapat
    CloseAudioDevice();

    // Pencereyi kapat
    CloseWindow();

    return 0;
}

// Başlangıç ekranını çizen ve kullanıcının oyunda ses isteyip istemediğini tuşlar ile kontrol eden fonksiyon
void DrawStartScreen() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // Çerçeve tasarımını yüklüyoruz
    startBackgroundTexture = LoadTexture(START_SCREEN_TEXTURE_FILENAME);
    // Arka plan resmi çiziliyor
    DrawTexture(startBackgroundTexture, 0, 0, WHITE);
    if (IsKeyPressed(KEY_C)) {
        StopSound(fxBackground);
        CloseAudioDevice();
    }
    else if (IsKeyPressed(KEY_B)) {
        InitAudioDevice();
        PlaySound(fxBackground);
    }
    EndDrawing();
}

void StartGame()
{
    // meyveler ekrana  yerleştirilmeden önce skor daima 0
    score = 0; //her oyun başlangıcında oyun sıfırlanır
    ReadHighScore();
   
    // Zamanlayıcıyı level 3 te  ve Cookie için kullanmak üzere başlatıyoruz
    srand(time(NULL));

    // Arka plan resmi yükleniyor
    backgroundTexture = LoadTexture(BACKGROUND_TEXTURE_FILENAME);

    // Arka plan ızgarası resmi yükleniyor ve pozisyonunu ayarlıyoruz
    backgroundGridTexture = LoadTexture(BACKGROUND_GRID_TEXTURE_FILENAME);
    backgroundGridPosX = (SCREEN_WIDTH - backgroundGridTexture.width) / 2;
    backgroundGridPosY = (SCREEN_HEIGHT - backgroundGridTexture.height) / 2;

    //meyvelerin resmi renk numaralarına göre eşleştiriliyor
    const char* filenames[NUM_COLORS]={BLUE_TEXTURE_FILENAME, RED_TEXTURE_FILENAME, GREEN_TEXTURE_FILENAME, YELLOW_TEXTURE_FILENAME};

    //Grid'i başlatmak için offset değerleri oluşturduk
    offsetX = (SCREEN_WIDTH - GRID_COLS * TILE_SIZE) / 2;
    offsetY = (SCREEN_HEIGHT - GRID_ROWS * TILE_SIZE) / 2;

    // Grid'i başlattık
    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            grid[row][col].coll = offsetX + col * TILE_SIZE;
            grid[row][col].roww = offsetY + row * TILE_SIZE;

            // rastgele bir renk seçiliyor
            randomColorIndex = GetRandomValue(0, NUM_COLORS - 1);
            grid[row][col].colorIndex = randomColorIndex;

            // Renk indexine göre o indexe daha önce eşleştirilen uygun meyve resmi yükleniyor
            // Yüklenen tasarım ilgili yere yerleştirildi
            grid[row][col].texture = LoadTexture(filenames[randomColorIndex]);

            // Tasarımın boyutu TILE_SIZE x TILE_SIZE piksel olarak ayarlandı
            grid[row][col].texture.width = TILE_SIZE;
            grid[row][col].texture.height = TILE_SIZE;
        }
    }
}

// SwitchTiles fonksiyonu sayesinde meyveler patladığı zaman kalan boşluklara üstünde kalan  meyveler düşüyor ve boşluklar kapanıyor
void SwitchTiles()
{
    // Her sütun için işlem yapıyoruz
    for (int col = 0; col < GRID_COLS; col++)
    {
        int emptySpaces = 0; // Boşluk sayısını kontrol eden değişken

        // Sütunu yukarıdan aşağıya tarayarak işlem yap
        for (int row = GRID_ROWS-1; row >=0; row--)
        {
            // Eğer hücre boşsa boşluk sayısını artırır (not: -1 boş hücrenin colorındexsi yani patlayan meyvelerin renk indexsleri de -1'e dönüşecek)
            if (grid[row][col].colorIndex == -1)
            {
                emptySpaces++;
            }
            // Eğer hücre doluysa
            else
            {
                // Eğer boşluk varsa, bu hücreyi boşlukların üstüne taşı
                if (emptySpaces > 0)
                {
                    // Şmeyveleri  aşağı doğru kaydır
                    grid[row + emptySpaces][col] = grid[row][col];
                    grid[row][col].colorIndex = -1; // Eski hücreyi boşalt
                }
            }
        }

        // Sütunun en üstündeki boşlukları doldur
        for (int i = 0; i < emptySpaces; i++)
        {
            // Rastgele bir renk yani meyve seç ve boş hücrelere yerleştir
            int randomIndex = GetRandomValue(0, NUM_COLORS - 1);

            // Dosya adını kullanarak resimlerini yükle
            grid[i][col].colorIndex = randomIndex;
            grid[i][col].texture = LoadTexture(filenames[randomIndex]);
        }
    }
}


//fonksiyon 3,4,5 li  eşleşmeleri kontrol ediyor, eşleşme sayısına göre puan veriyor ve 5'li eşleşmeler Cookie özel ögesini çıkarıyor
bool MatchAndCreate()
{
    bool matchFound = false; // Fonksiyonun herhangi bir eşleşme bulup bulmadığı izler
    static Vector2 lastMousePosition = {-1, -1};

    // Satırlardaki ve sütunlardaki eşleşmeleri kontrol et
    for (int row = 0; row < GRID_ROWS; row++) // Satırları tarar
    {  
        for (int col = 0; col < GRID_COLS; col++) // Sütunları tarar
        { 
            int match = 1; // Eşleşmenin boyutu yani kaç tane aynı renk meyvenin yan yana veya alt alta olduğu

            // Yatay eşleşmeleri kontrol et
            while (col + match < GRID_COLS && grid[row][col].colorIndex == grid[row][col + match].colorIndex)
            {
                match++; // Taranan hücrenin sağındaki hücreye geçilir ve eğer aynı renkte ardışık hücre varsa match artırılır
            }
            if (match >= 3)
            {
                matchFound = true;
                for (int i = 0; i < match; i++) // i değişkeni eşleşen hücrelerin indislerini temsil eder
                {
                    grid[row][col + i].colorIndex = -1; // Renk indexi boş hücre haline gelir yani eşleşen meyveler patlıyor ve yerleri boşalıyor
                }

                //eşleşme sayısına göre farklı puanlar atanıyor
                if (matchFound) {  
                    if (match == 3) {
                        score += MATCH_SCORE;
                        SwitchTiles();
                    } 
                    if (match == 4) {
                        score += MATCH_SCORE + 5;
                        SwitchTiles();
                    }
                     if (match == 5) {
                        score += MATCH_SCORE + 10;
                        SwitchTiles();
                    }
                }
            }

            // Eğer 5 veya daha fazla aynı renkte kare yan yana veya alt alta ise cookieyi çıkarıyor
            if (match >= 5) {
                //5 li eşleşme bulundu şimdi yatayda mı dikeyde mi onu öğrenmeye çalışıyor
                if (col + match / 2 < GRID_COLS) { // Eğer yatay eşleşme ise
                    grid[row][col + match / 2].colorIndex = NUM_COLORS; // NUM_COLORS siyah rengi temsil eder yani Cookie'yi
                    matchFound = true; // Siyah kare oluşturulduğunda bir eşleşme bulundu olarak işaretliyor
                    cookies++;
                    grid[row][col + match / 2].creationTime = time(NULL); // Zaman bilgisini saklıyor çünkü cookie oluştktan belli bir süre sonra patlar

                    // Siyah kare tasarımını yüklüyor yani bir kurabiye tasarımı
                    grid[row][col + match / 2].texture = LoadTexture(COOKIE_TEXTURE_FILENAME);

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        SoundInput2(lastMousePosition);
                    }

                } else if (row + match / 2 < GRID_ROWS) { // Eğer dikey eşleşme ise
                    grid[row + match / 2][col].colorIndex = NUM_COLORS; // NUM_COLORS siyah rengi temsil eder yani Cookie
                    matchFound = true; // Siyah kare oluşturulduğunda bir eşleşme bulundu olarak işaretliyor
                    cookies++;
                    grid[row + match / 2][col].creationTime = time(NULL); // Zaman bilgisini saklıyor çünkü cookie oluştuktan belli bir süre sonra patlar

                    // Siyah kare tasarımını yüklüyor yani kurabiye tasarımı 
                    grid[row + match / 2][col].texture = LoadTexture(COOKIE_TEXTURE_FILENAME);

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        SoundInput2(lastMousePosition);
                    }
                }
            }

            // Dikey eşleşmeleri kontrol et
            match = 1;
            while (row + match < GRID_ROWS && grid[row][col].colorIndex == grid[row + match][col].colorIndex)
            {
                match++;
            }
            if (match >= 3)
            {
                matchFound = true;
                for (int i = 0; i < match; i++)
                {
                    grid[row + i][col].colorIndex = -1;
                }
                if (matchFound) {
                    if (match == 3) {
                        score += MATCH_SCORE;
                        SwitchTiles();
                    }
                    if (match == 4) {
                        score += MATCH_SCORE + 5;
                        SwitchTiles();
                    }
                    if (match == 5) {
                        score += MATCH_SCORE + 10;
                        SwitchTiles();
                    }
                }
            }

            // Eğer 5 veya daha fazla aynı renkte kare yan yana veya alt alta ise cookie oluşacak
            if (match >= 5) {
                // 5 li eşleşme var cookie yapıcaz
                if (row + match / 2 < GRID_ROWS) { // Eğer dikey eşleşme ise
                    grid[row + match / 2][col].colorIndex = NUM_COLORS; // NUM_COLORS siyah rengi temsil eder yani cookiyi
                    matchFound = true; // Siyah kare oluşturulduğunda bir eşleşme bulundu olarak işaretle
                    cookies++;
                    grid[row + match / 2][col].creationTime = time(NULL); // Zaman bilgisini sakla çünkü cookie oluştuktan birkaç saniye sonra patlıyor 

                    // Siyah kare tasarımını yani kurabiye tasarımını yükle
                    grid[row + match / 2][col].texture = LoadTexture(COOKIE_TEXTURE_FILENAME);

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        SoundInput2(lastMousePosition);
                    }

                } else if (col + match / 2 < GRID_COLS) { // Eğer yatay eşleşme ise
                    grid[row][col + match / 2].colorIndex = NUM_COLORS; // NUM_COLORS siyah rengi yani cookimizi temsil eder
                    matchFound = true; // Siyah kare oluşturulduğunda bir eşleşme bulundu olarak işaretle
                    cookies++;
                    grid[row][col + match / 2].creationTime = time(NULL); // Zaman bilgisini sakla çünkü cookie oluştuktan belli bir süre sonra patlar

                    // Siyah kare tasarımını yani kurabiye tasarımını yükle
                    grid[row][col + match / 2].texture = LoadTexture(COOKIE_TEXTURE_FILENAME);

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        SoundInput2(lastMousePosition);
                    }
                }
            }
        }
    }

    // ekranda görünen Cookie sayacını kontrol et
    if (cookies >= 1)
    {
        cookieCounter++; // Cookie sayacını artır
        cookies = 0; // Cookie sayısını sıfırla
    }

    return matchFound;
}


//fare hareketleri kontrol ediliyor bu fonksiyon sayesinde meyveler sürüklenebiliyor ve yer değiştirebiliyor
void UpdateGame() {
    

    Vector2 mousePosition = GetMousePosition();
    int col = (mousePosition.x - (GetScreenWidth() - GRID_COLS * TILE_SIZE) / 2) / TILE_SIZE;
    int row = (mousePosition.y - (GetScreenHeight() - GRID_ROWS * TILE_SIZE) / 2) / TILE_SIZE;
  

  //mouse a basıldı yani bir meyve sürükleniyor demek, sürüklüyoruz..
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!isDragging && col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
            lastMousePosition = mousePosition;
            startingCol = col;
            startingRow = row;
            draggingTile = grid[row][col]; // Sürüklenen meyve bilgilerini al
            draggingRect = (Rectangle){mousePosition.x - TILE_SIZE / 2, mousePosition.y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
            isDragging = true;
        }
    } 
    //mouse serbest bırakıldı yani meyvelerin yerini değiştiriyoruz ve eşleşme kontrolü yapıyoruz
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (isDragging) {

            //meyveler yer değiştiriyor eşleşme varsa en sonda MatchandCreate ile patlatılıyor
            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS &&
                abs(startingCol - col) + abs(startingRow - row) == 1) {
                Tile temp = grid[row][col];
                grid[row][col] = grid[startingRow][startingCol];
                grid[startingRow][startingCol] = temp;

                bool matchFound = MatchAndCreate();

                //eşleşme yoksa meyveler hemenn eski yerlerine geri çiziliyor anlık olduğu için  gözle görmek pek mümkün değil 
                if (!matchFound) {
                    temp = grid[row][col];
                    grid[row][col] = grid[startingRow][startingCol];
                    grid[startingRow][startingCol] = temp;
                }
                
                //eşleşme varsa hamle sayısını azaltmayı unutmuyoruz
                 else {
                    movesLeft--;
                }
            }
            isDragging = false;
        }
    }

    // Sürüklenen şmeyveleri görsel olarak hareket ettiriyoruz
    if (isDragging) {
        draggingRect.x = mousePosition.x - TILE_SIZE / 2;
        draggingRect.y = mousePosition.y - TILE_SIZE / 2;
    }

    if (isDragging && (mousePosition.x != lastMousePosition.x || mousePosition.y != lastMousePosition.y)) {
        lastMousePosition = mousePosition;
    }

    //eşleşme kontrolü yaılıyor eşleşme varsa patlayan meyveler sonucu oluşan boşlukları switchTiles fonksiyonu dolduracak
    bool matchFound = MatchAndCreate();
    if (matchFound) {
        SwitchTiles();
    }

    SoundInput(lastMousePosition);
    CheckSpecialTiles(grid);
    CheckFallenTiles(grid);
}


// Ses girişini işleyen fonksiyonumuz ses ise meyveler eşleştiği zaman çıkan ses
void SoundInput(Vector2 lastMousePosition) {
    fxTap = LoadSound("assets/tap-notification-180637.wav");
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        PlaySound(fxTap);
    }
}

// Ses girişini işleyen başka bir fonskiyon ses ise cookie oluştuğunda çıkan ona özel bir ses
void SoundInput2(Vector2 lastMousePosition) {
    Sound fxCookiePop= LoadSound("assets/bonus-points-190035.wav");
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        PlaySound(fxCookiePop);
    }
}

// Her çerçeve sonunda cookielerin kontrolü yapılır ve patladıktan sonra kendisine komuşu meyveleri patlatması sağlanır. bu cookie'nin özel gücü
void CheckSpecialTiles(Tile grid[GRID_ROWS][GRID_COLS]) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {

            //cookie oluştuktan sonra patlamak için 3 saniye bekliyor
            if (grid[row][col].colorIndex == NUM_COLORS && difftime(time(NULL), grid[row][col].creationTime) >= 3) {
                grid[row][col].colorIndex = -1;

                // Cookie'ye özel  tasarım yükleniyor
                if (grid[row][col].cookieTile) {
                    grid[row][col].texture = LoadTexture(COOKIE_TEXTURE_FILENAME);
                }
                
                // Çevredeki meyveler işaretlenip patlatılıyor
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int newRow = row + i;
                        int newCol = col + j;

                        if (newRow >= 0 && newRow < GRID_ROWS && newCol >= 0 && newCol < GRID_COLS &&
                            !(i == 0 && j == 0) && grid[newRow][newCol].colorIndex != -1) {
                            grid[newRow][newCol].colorIndex = -2;
                        }
                    }
                }
            }
        }
    }

    // İşaretlenmiş meyveleri temizle
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (grid[row][col].colorIndex == -2) {
                grid[row][col].colorIndex = -1;
            }
        }
    }
}

// Düşmüş meyveleri güncelle
void CheckFallenTiles(Tile grid[GRID_ROWS][GRID_COLS]) {
    for (int col = 0; col < GRID_COLS; col++) {
        int offset = 0;
        for (int row = GRID_ROWS - 1; row >= 0; row--) {
            if (grid[row][col].colorIndex != -1) {
                grid[row + offset][col] = grid[row][col];

                if (offset != 0) {
                    grid[row][col].colorIndex = -1;
                }
            } else {
                offset++;
            }
        }
    }
}

void DrawingGame(int currentLevel) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    DrawingBackground();
    DrawingGrid(grid);
    DrawingLevels();

    // Sürüklenen meyveyi çiz
    if (isDragging) {
        DrawTexturePro(draggingTile.texture, 
                       (Rectangle){0, 0, draggingTile.texture.width, draggingTile.texture.height}, 
                       (Rectangle){draggingRect.x, draggingRect.y, TILE_SIZE, TILE_SIZE}, 
                       (Vector2){0, 0}, 0, WHITE);
    }
    CreateLevelObjectives(currentLevel);

    EndDrawing();
}


void DrawingBackground() {
    DrawTexturePro(backgroundTexture, (Rectangle){0, 0, backgroundTexture.width, backgroundTexture.height},
                   (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, (Vector2){0, 0}, 0, WHITE);

    // Çerçeveyi ekranın tam ortasına yerleştir
    int offsetX = (GetScreenWidth() - BACKGROUND_GRID_WIDTH) / 2;
    int offsetY = (GetScreenHeight() - BACKGROUND_GRID_HEIGHT) / 2;

    // Çerçeveyi çiz
    DrawTexture(backgroundGridTexture, offsetX, offsetY, WHITE);

}

void DrawingGrid(Tile grid[GRID_ROWS][GRID_COLS]) {
    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            if (grid[row][col].colorIndex != -1)
            {
                DrawTexturePro(grid[row][col].texture, (Rectangle){0, 0, grid[row][col].texture.width, grid[row][col].texture.height}, 
                                (Rectangle){col * TILE_SIZE + (SCREEN_WIDTH - GRID_COLS * TILE_SIZE) / 2, row * TILE_SIZE + (SCREEN_HEIGHT - GRID_ROWS * TILE_SIZE) / 2, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

//her levelde ekrana yazılan farklı görev ve sayaçların yapılıdğı fonksiyon 
void DrawingLevels() {
    
    ReadHighScore(); // En yüksek skor değişkeni sadece level 2 için kullanılır

    // level 2 de High Score'u kontrol et ve druruma göre güncelle
    if ( currentLevel==2 && score  > highScore) {
        highScore = score; 
        // skoru kırdıysak yeni skoru yüksek skor olarak ayarla. bu değişikler kırılan skorun bir dosyaya yazılması ve oradan okunması ile yapılıyor

        //highscoru dosyaya yazdıran fonksiyon 
        WriteHighScore();
    }


    //score ve movesleft ekranın sol üst köşesine  yazdırılıyor
    DrawText(TextFormat("Score: %d", score), 10, 10, 30, BLACK);
    DrawText(TextFormat("Moves Left: %d", movesLeft), 10, 40, 30, BLACK);
    
    //her level için farklı görevler ekrana yazdırılıyor
    if (currentLevel == 1) {
        DrawText(TextFormat("Level %d: Achieve 1000 points to win.", currentLevel), 10, 70, 20, BLACK);
    } else if (currentLevel == 2) {
        DrawText(TextFormat("Level %d: Pop 4 cookies to win. (Cookies popped: %d)", currentLevel, cookieCounter), 10, 70, 20, BLACK);
        DrawText(TextFormat("High Score: %d", highScore),  SCREEN_WIDTH - 200, 40, 20, BLACK);

    } else if (currentLevel == 3) {
        DrawText(TextFormat("Level %d: Achieve 1300 points in 60 seconds to win.", currentLevel), 10, 70, 20, BLACK);
        DrawText(TextFormat("Time Left: %.0f", 60 - (GetTime() - level3Timer)), SCREEN_WIDTH - 150, 40, 20, BLACK);
    } else {
        DrawText(TextFormat("Level %d: Achieve %d points  to win.", currentLevel, currentLevel*350), 10, 100, 20, BLACK);
    }
}

void CreateLevelObjectives(int currentLevel) {
    switch (currentLevel) {
        case 1:
            CheckLevel1Objectives();
            break;
        case 2:
            CheckLevel2Objectives();
            break;
        case 3:
            CheckLevel3Objectives();
            break;
        default:
            CheckLevelObjectives();
    }
}


// bu alt alta olan 4 fonskiyon leveller için geçiş şartını belirliyor ve kullanıcının leveli kaybedip R ye basması durumunda leveli baştan başlatıyor.
//Leveli kazanıp N ye basılması durumunda currentlevel'i 1 arttırarak bir sonraki levele geçiliyor
void CheckLevel1Objectives() {
    if (score >= 1000) {
        ShowHighScore();
        if (IsKeyPressed(KEY_N)) {
            currentLevel++;
            CreateLevel(currentLevel);
        } 
    }else if (movesLeft == 0) {
        ShowFailMessage();
        if (IsKeyPressed(KEY_R)) {
            CreateLevel(currentLevel);
        }
    }
}


void CheckLevel2Objectives() {
        if (cookieCounter>= 4) {
            ShowHighScore();
            if (IsKeyPressed(KEY_N)) {
                currentLevel++;
                CreateLevel(currentLevel);
            }
        } else if (movesLeft == 0) {
            ShowFailMessage();
        
        if (IsKeyPressed(KEY_R)) {
                CreateLevel(currentLevel);
        }
    }
}

void CheckLevel3Objectives() {
  ; // Geriye kalan süreyi hesapla
    if (score >= 1300 && 60 - (GetTime() - level3Timer> 0)) {
        ShowHighScore();
        if (IsKeyPressed(KEY_N)) {
            currentLevel++;
            CreateLevel(currentLevel);
        }
    } else if (60 - (GetTime() - level3Timer) < 0 || movesLeft == 0) {
        if (!level3TimerStart) {
            level3TimerStart = true;
            level3Timer = GetTime();
        }
        ShowFailMessage();
        if (IsKeyPressed(KEY_R)) {
            CreateLevel(currentLevel);
        }
    }
}


// 5. levelden sonra oyunu bitiriyor.
void CheckLevelObjectives() {
     if (movesLeft == 0) {
        ShowFailMessage();
        if (IsKeyPressed(KEY_R)) {
            CreateLevel(currentLevel);
        }
    } else if(score >= currentLevel*350 && currentLevel<=5){
        
        if(currentLevel==5){     
            ShowGameEndMessage();
        }

        else {ShowHighScore();}

        if (IsKeyPressed(KEY_N)) {
            currentLevel++;
            CreateLevel(currentLevel);
        }
    }
}

//yenilince çıkan arkaplan resmi
void ShowFailMessage() {
    failBackgroundTexture = LoadTexture(YOULOST_SCREEN_TEXTURE_FILENAME);
    // Arka plan resmini ekrana çiz
    DrawTexture(failBackgroundTexture, 0, 0, WHITE);
}


//highscorun ekrana yazılış ayarları yapılıyor. 2. levelde high skorun kırılıp kırılmadığına bağlı olarak farklı kazanma ekranları karşına çıkartır
void ShowHighScore() {

    // Çerçeve tasarımını yükle
    level2BackgroundTexture = LoadTexture(BROKETHEHIGH_SCREEN_TEXTURE_FILENAME);
    // Arka plan resmini ekrana çiz
    DrawTexture(level2BackgroundTexture, 0, 0, WHITE);
 
    if (currentLevel == 2 && score >= highScore) {
    DrawText(TextFormat("High Score: %d", highScore), SCREEN_WIDTH / 2 - 100, 20, 30, BLACK);
}
else{
    // Çerçeve tasarımını yükle
    levelBackgroundTexture = LoadTexture(YOUWIN_SCREEN_TEXTURE_FILENAME);
    // Arka plan resmini ekrana çiz
    DrawTexture(levelBackgroundTexture, 0, 0, WHITE);
}
}

void ShowGameEndMessage() {
   // Çerçeve tasarımını yükle
    gameOverBackgroundTexture = LoadTexture(GAMEOVER_SCREEN_TEXTURE_FILENAME);
    // Arka plan resmini ekrana çiz
    DrawTexture(gameOverBackgroundTexture, 0, 0, WHITE);
}


// Level ve zorluk seviyesini ayarlamak için fonksiyon
void CreateLevel(int currentLevel)
{
    // İlk olarak, oyunun başlangıcında skor 0 olsun
    score = 0;
    
    if(currentLevel ==1){

        movesLeft = MAX_MOVES;
    }if(currentLevel ==2){

        movesLeft = MAX_MOVES-5;
        cookieCounter=0;
        cookies=0;
    }
        level3TimerStart= false;
        

        if(currentLevel==3){

        movesLeft = MAX_MOVES-10;
            if (!level3TimerStart)
            {
                level3TimerStart = true;
                level3Timer = GetTime();
            }
        }else if(currentLevel >= 4){
        // Her seviyede hamle sayısını önceki seviyeden 5 azalt
        movesLeft = MAX_MOVES - (currentLevel - 1) * 5;
    }   

        // Grid'i başlangıç durumuna getir
        StartGame();

        // Seviye hedeflerini ayarla
        CreateLevelObjectives(currentLevel);

}