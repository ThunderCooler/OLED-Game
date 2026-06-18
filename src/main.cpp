#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

int y = 40;
int x = 59;
int AY = 10;
int unsigned i = 0;
int unsigned limit = 0;
int step = 10;
int last;
bool IsBullet = true;
bool IsAmmo, Hit = false;
int randomValue;
int ammoCount = 0;
int currentRandX = 0;
int currentRandY = 0;
bool spawn = true;
bool allinactive = false;
int spawntimer = 0;
int points = 0;
bool hit = false;
char selected = 'n';
char c = ' ';
int maxenemies = 1;
int wavenum = 1;

const static uint8_t BMEnemy[] PROGMEM = {
  0b11100000, 0b00000111,
  0b01010000, 0b00001010,
  0b00001100, 0b00110000,
  0b01010010, 0b01001010,
  0b11100001, 0b10000111,
  0b00010001, 0b10001000,
  0b00001000, 0b00010000,
  0b00001000, 0b00010000 
};

const static uint8_t Player[] PROGMEM = {
  0b10000000, 0b00000001,    
  0b10010000, 0b00001001,   
  0b10010000, 0b00001001,   
  0b11110000, 0b00001111,  
  0b11110000, 0b00001111,   
  0b11111000, 0b00011111,   
  0b01110000, 0b00001110,     
  0b01100000, 0b00000110
};

class Bullet {
  public:
    int X;
    int Y;
    int cooldown = random(20) + 10;

    void DrawBullet() {
      u8g2.drawBox(X, Y, 4, 4);
    }
    void DrawDeathOrb() {
      u8g2.drawFrame(X, Y, 4, 4);
    }
};
Bullet bullets[5];

class Enemy {
  public:
    int X;
    int Y;
    bool active = false;
    int steps = 5;
    bool IsRight;
    int stepsCount;
    Bullet deathorb;
    
    void DrawEnemy() {
      u8g2.drawXBMP(X, Y, 16, 8, BMEnemy);
    }

    void EnemyMove() {
    if(stepsCount <= 0) IsRight = true;
    if(stepsCount >= steps) IsRight = false;
    if(IsRight) { 
      stepsCount++;
      X++;
    }
    else {
      stepsCount--;
      X--;
    }
  }
};
Enemy enemies[10];

void Reset() {
  bullets[0].X = 150;
  bullets[0].Y = 100;
  y = 40;
  x = 59;
  AY = 10;
  i = 0;
  limit = 0;
  step = 10;
  IsBullet = true;
  IsAmmo = false;
  Hit = false;
  ammoCount = 0;
  currentRandX = 0;
  currentRandY = 0;
  spawn = true;
  allinactive = false;
  spawntimer = 0;
  points = 0;
  maxenemies = 1;
  wavenum = 1;
  for (unsigned int l = 0; l < sizeof(bullets) / sizeof(bullets[0]); l++) {
    bullets[l].X = 150;
    bullets[l].Y = 100;
  }
  for (int m = 0; m < maxenemies; m++) {
    enemies[m].active = false;
    enemies[m].deathorb.cooldown = random(20) + 10;
  }
}

void menu() {
  while(c != 'c' || selected != 's') {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(36, 15, "SPACE");
    u8g2.drawStr(19, 30, "INVADERS");
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(52, 44, "Start");
    u8g2.drawStr(52, 54, "Quit");
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(30, 64, "Press c to select");
    if (Serial.available() > 0) {
      c = Serial.read();
      Serial.println(c);
    }
    if (c == 'w' || selected == 's') {
      u8g2.drawTriangle(42, 37, 48, 41, 42, 45); 
      selected = 's';
    }
    if (c == 's' || selected == 'q') {
      u8g2.drawTriangle(42, 47, 48, 51, 42, 55); 
      selected = 'q';
    }
    if (c == 'c' && selected == 'q') Reset(); 
    u8g2.sendBuffer();
  }
}

void DIE() {
  while(c != 'r' && c != 'm') {
    u8g2.setColorIndex(1);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tr); //u8g2_font_ncenB08_tr
    u8g2.drawStr(32, 35, "GAME OVER");
    u8g2.sendBuffer();
    c = Serial.read();
    Serial.println(c);
  }
  if (c == 'r') Reset();
  if (c == 'm') menu();
}

bool IsDifferent() {
  bool result = true;
  for (int b = 0; b < maxenemies - 1; b++) {
    if (currentRandX >= enemies[b].X - 16 && currentRandX <= enemies[b].X + 16 && currentRandY >= enemies[b].Y - 8 && currentRandY <= enemies[b].Y + 8) result = false;  
  }
  return result;
}

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  bullets[0].X = 150;
  bullets[0].Y = 100;
  menu();
}

void loop() {
 if (Serial.available() > 0) {
    c = Serial.read();
    if (c == 'w' && y > 10) y-=step;
    if (c == 's' && y < 50) y+=step;
    if (c == 'a' && x > 9) x-=step;
    if (c == 'd' && x < 115) x+=step;
    if (c == 'r') Reset();
    if (c == 'm') menu();
    if (c == 'x' && IsBullet) {
      bullets[limit].Y = y;
      bullets[limit].X = x + 6;
      limit++;
    }
    Serial.println(c);
  }
  u8g2.setColorIndex(1);
  u8g2.clearBuffer();
  u8g2.drawXBMP(x, y, 16, 8, Player);
  if (spawn) spawntimer++;
  if (spawntimer > 50) {
    maxenemies++;
    wavenum++;
    for (int c = 0; c < maxenemies; c++) {
      do {
        currentRandX = random(100) + 10;
        currentRandY = random(15) + 10;
      } while (!IsDifferent());

      enemies[c].X = currentRandX;
      enemies[c].Y = currentRandY;
      enemies[c].active = true;
      enemies[c].deathorb.cooldown = random(20) + 10;
    }
  spawn = false;
  spawntimer = 0;
  }
  for (int k = 0; k < maxenemies; k++) {
    if (enemies[k].active) {
      enemies[k].DrawEnemy();
      enemies[k].EnemyMove();
    }
    if (enemies[k].active) enemies[k].deathorb.cooldown--;
    if (enemies[k].deathorb.cooldown > 0) {
      enemies[k].deathorb.X = enemies[k].X + 7;
      enemies[k].deathorb.Y = enemies[k].Y + 3;
    }
    if (enemies[k].deathorb.cooldown <= 0) {
      enemies[k].deathorb.Y += 4;
      enemies[k].deathorb.DrawDeathOrb();
    }
    if (enemies[k].deathorb.Y >= 70) enemies[k].deathorb.cooldown = random(20) + 10;
    if (enemies[k].active && ((y <= enemies[k].Y + 5 && x >= enemies[k].X - 7 && x <= enemies[k].X + 15) || (y <= enemies[k].deathorb.Y + 4 && y >= enemies[k].deathorb.Y - 4 && x >= enemies[k].deathorb.X - 13 && x <= enemies[k].deathorb.X + 2))) DIE();
  }
  while (i < limit) {
    if (bullets[i].Y >= 5) bullets[i].DrawBullet();
    for (int j = 0; j < maxenemies; j++) {
      if (bullets[i].Y < enemies[j].Y + 8 && bullets[i].Y > enemies[j].Y - 3 && bullets[i].X > enemies[j].X - 3 && bullets[i].X < enemies[j].X + 15 && enemies[j].active) {
        enemies[j].active = false;
        hit = true;
      }
    }
    bullets[i].Y -= 4;
    last = i;
    i++;
  }
  if (hit) {
    points++;
    hit = false;
  }
  allinactive = true;
  for (int k = 0; k < maxenemies; k++) {
    if (enemies[k].active) {
      allinactive = false;
      break;
    }
  }
  if (allinactive) {
    spawn = true;
    u8g2.setFont(u8g2_font_courB10_tr);
    String waveStr = "WAVE:" + String(wavenum);
    u8g2.drawStr(35, 25, waveStr.c_str());
  }
  i = 0;
  if (limit >= sizeof(bullets) / sizeof(bullets[0])) IsBullet = false;
  if (!IsBullet) ammoCount++;
  if (ammoCount == 50) {
    IsAmmo = true;
    randomValue = random(100) + 10;
  }
  if (IsAmmo) {
    u8g2.drawFilledEllipse(randomValue, AY, 5, 5);
    AY++;
  }
  if (AY > 80) {
    randomValue = random(100) + 10;
    AY = 10;
  }
  if ((y >= AY - 10 && y <= AY + 3) && (x >= randomValue - 15 && x <= randomValue + 3)) {
    IsAmmo = false;
    IsBullet = true;
    AY = 10;
    limit = 0;
    ammoCount = 0;
  }
  u8g2.setFont(u8g2_font_5x7_tf);
  String pointsStr = "Points: " + String(points);
  u8g2.drawStr(42, 7, pointsStr.c_str());
  u8g2.sendBuffer();
  delay(50);
}