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
char c;
// static unsigned long t = 0;

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
    int cooldown = random(30);

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
Enemy enemies[3];

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
  for (unsigned int l = 0; l < sizeof(bullets) / sizeof(bullets[0]); l++) {
    bullets[l].X = 150;
    bullets[l].Y = 100;
  }
  for (unsigned int m = 0; m < sizeof(enemies) / sizeof(enemies[0]); m++) {
    enemies[m].active = false;
    enemies[m].deathorb.cooldown = random(30);
  }
}

void DIE() {
  while(c != 'r') {
    u8g2.setColorIndex(1);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tr); //u8g2_font_ncenB08_tr
    u8g2.drawStr(35, 35, "GAME OVER");
    u8g2.sendBuffer();
    c = Serial.read();
    Serial.println(c);
  }
  Reset();
}

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  bullets[0].X = 150;
  bullets[0].Y = 100;
}

void loop() {
 if (Serial.available() > 0) {
    c = Serial.read();
    if (c == 'w' && y > 10) y-=step;
    if (c == 's' && y < 50) y+=step;
    if (c == 'a' && x > 9) x-=step;
    if (c == 'd' && x < 115) x+=step;
    if (c == 'r') Reset();
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
  for (unsigned int k = 0; k < sizeof(enemies) / sizeof(enemies[0]); k++) {
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
    if (enemies[k].deathorb.Y >= 70) enemies[k].deathorb.cooldown = random(30);
    if (enemies[k].active && ((y <= enemies[k].Y + 5 && y >= enemies[k].Y - 3 && x >= enemies[k].X - 7 && x <= enemies[k].X + 15) || (y <= enemies[k].deathorb.Y + 4 && y >= enemies[k].deathorb.Y - 4 && x >= enemies[k].deathorb.X - 13 && x <= enemies[k].deathorb.X + 2))) DIE();
  }
  while (i < limit) {
    if (bullets[i].Y >= -10) bullets[i].DrawBullet();
    for (unsigned int j = 0; j < sizeof(enemies) / sizeof(enemies[0]); j++) {
      if (bullets[i].Y < enemies[j].Y + 8 && bullets[i].Y > enemies[j].Y - 3 && bullets[i].X > enemies[j].X - 3 && bullets[i].X < enemies[j].X + 15) {
        enemies[j].active = false;
      }
    }
    bullets[i].Y -= 4;
    last = i;
    i++;
  }
  allinactive = !enemies[0].active && !enemies[1].active && !enemies[2].active;
  if (allinactive) spawn = true;
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
  if (spawn) spawntimer++;
  if (spawntimer > 50) {
    for (unsigned int c = 0; c < sizeof(enemies) / sizeof(enemies[0]); c++) {
      do {
        currentRandX = random(100) + 10;
        currentRandY = random(20) + 5;
      } while (
        (currentRandX >= enemies[c - 1].X - 15 && currentRandX <= enemies[c - 1].X + 15 && currentRandY >= enemies[c - 1].Y - 10 && currentRandY <= enemies[c - 1].Y + 10) ||
        (currentRandX >= enemies[c - 2].X - 15 && currentRandX <= enemies[c - 2].X + 15 && currentRandY >= enemies[c - 2].Y - 10 && currentRandY <= enemies[c - 2].Y + 10)
      );

      enemies[c].X = currentRandX;
      enemies[c].Y = currentRandY;
      enemies[c].active = true;
  }
  spawn = false;
  spawntimer = 0;
  }
  u8g2.sendBuffer();
  // if (millis() - t > 1000) {
  //   t = millis();
  //   Serial.println(allinactive);
  // }
  delay(50);
}