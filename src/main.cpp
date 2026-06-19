#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); // neccessary line to use u8g2

//--Variables--
int PlayerY = 40;
int PlayerX = 59;
int AmmoY = 10;
int unsigned BulletCounter = 0;
int unsigned Bulletslimit = 0;
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
char myChar = ' ';
int maxenemies = 1;
int wavenum = 1;

//--Bitmaps--
const static uint8_t BMEnemy[] PROGMEM = { // bitmap design for enemy, needs to be sliced and swaped in order to work properly
  0b11100000, 0b00000111,
  0b01010000, 0b00001010,
  0b00001100, 0b00110000,
  0b01010010, 0b01001010,
  0b11100001, 0b10000111,
  0b00010001, 0b10001000,
  0b00001000, 0b00010000,
  0b00001000, 0b00010000 
};

const static uint8_t Player[] PROGMEM = { // bitmap design for player
  0b10000000, 0b00000001,    
  0b10010000, 0b00001001,   
  0b10010000, 0b00001001,   
  0b11110000, 0b00001111,  
  0b11110000, 0b00001111,   
  0b11111000, 0b00011111,   
  0b01110000, 0b00001110,     
  0b01100000, 0b00000110
};

const static uint8_t MotherShip[] PROGMEM = {
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b11100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00111000, 0b00011100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,  
  0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b11000111, 0b11100011, 0b10000000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00111111, 0b11111100, 0b11000000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b11000111, 0b11111111, 0b01100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b10001111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b00011111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b00111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b01111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b11111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b11111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b11111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b11111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b11111111, 0b11111111, 0b10100000, 0b00000000, 0b00000000, 0b00000000,
  0b00111111, 0b11111111, 0b11111111, 0b11111000, 0b11111111, 0b11111111, 0b00011111, 0b11111111, 0b11111111, 0b11111100,
  0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00111111, 0b11111100, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
  0b10011000, 0b00000000, 0b00000000, 0b00000000, 0b00001111, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00011001,
  0b01011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011010,
  0b00100000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00000100,
  0b00011000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00011000,
  0b00000110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01100000,
  0b00000001, 0b10000000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00000001, 0b10000000,
  0b00000000, 0b01110000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001110, 0b00000000,
  0b00000000, 0b00001110, 0b00000000, 0b00110000, 0b00000000, 0b00000000, 0b00001100, 0b00000000, 0b01110000, 0b00000000,
  0b00000000, 0b00000001, 0b11000000, 0b00110000, 0b00011000, 0b00011000, 0b00001100, 0b00000011, 0b10000000, 0b00000000,
  0b00000000, 0b00000000, 0b00111111, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b11111100, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

//--Classes--
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

//--Functions--
void CheckNDrawByte(uint16_t X, uint16_t Y, const uint8_t* data, int MaxRow, int MaxColumn) { // draws each box for Bitmap image, needed as the library function sucks 
  for(int z = 0; z < MaxRow; z++){
    for (int y = 0; y < MaxColumn; y++){
      uint8_t CurrentByte = pgm_read_byte(&data[z * MaxColumn + y]);
      if (CurrentByte != 0) {
        uint8_t CompareByte = 128;
        for (int bit = 0; bit < 8; bit++) {
          if ((CurrentByte & CompareByte) > 0) u8g2.drawBox(X + y * 8 + bit, Y + z, 1, 1);
          CompareByte >>= 1;
        }
      }
    }
  }
}

void Reset() { // reset all values
  bullets[0].X = 150;
  bullets[0].Y = 100;
  PlayerY = 40;
  PlayerX = 59;
  AmmoY = 10;
  BulletCounter = 0;
  Bulletslimit = 0;
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
  selected = 'n';
  myChar = ' ';
  for (unsigned int l = 0; l < sizeof(bullets) / sizeof(bullets[0]); l++) {
    bullets[l].X = 150;
    bullets[l].Y = 100;
  }
  for (int m = 0; m < maxenemies; m++) {
    enemies[m].active = false;
    enemies[m].deathorb.cooldown = random(20) + 10;
  }
}

void menu() { // create menu screen
  Reset();
  while(myChar != 'c' || selected != 's') {
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
      myChar = Serial.read();
      Serial.println(myChar);
    }
    if (myChar == 'w' || selected == 's') {
      u8g2.drawTriangle(42, 37, 48, 41, 42, 45); 
      selected = 's';
    }
    if (myChar == 's' || selected == 'q') {
      u8g2.drawTriangle(42, 47, 48, 51, 42, 55); 
      selected = 'q';
    }
    if (myChar == 'c' && selected == 'q') Reset(); 
    u8g2.sendBuffer();
  }
}

void DIE() { // Kill Player
  while(myChar != 'r' && myChar != 'm') {
    u8g2.setColorIndex(1);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tr); //u8g2_font_ncenB08_tr
    u8g2.drawStr(32, 35, "GAME OVER");
    u8g2.sendBuffer();
    myChar = Serial.read();
    Serial.println(myChar);
  }
  if (myChar == 'r') Reset();
  if (myChar == 'm') menu();
}

bool IsDifferent() { // compare each enemy X and Y cord to ensure they are different
  bool result = true;
  for (int b = 0; b < maxenemies - 1; b++) {
    if (currentRandX >= enemies[b].X - 16 && currentRandX <= enemies[b].X + 16 && currentRandY >= enemies[b].Y - 8 && currentRandY <= enemies[b].Y + 8) result = false;  
  }
  return result;
}

void VerticalMove(int &y, int a, int b, int speed) {
  y -= speed;
  if (y < a) y = b;
  if (y > b) y = a;
}

void FINALE() {
  
}

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  bullets[0].X = 150;
  bullets[0].Y = 100;
  menu();
}

void loop() {
 if (Serial.available() > 0) { // take Input from user
    myChar = Serial.read();
    if (myChar == 'w' && PlayerY > 10) PlayerY-=step;
    if (myChar == 's' && PlayerY < 50) PlayerY+=step;
    if (myChar == 'a' && PlayerX > 9) PlayerX-=step;
    if (myChar == 'd' && PlayerX < 115) PlayerX+=step;
    if (myChar == 'r') Reset();
    if (myChar == 'm') menu();
    if (myChar == 'x' && IsBullet) {
      bullets[Bulletslimit].Y = PlayerY;
      bullets[Bulletslimit].X = PlayerX + 6;
      Bulletslimit++;
    }
    Serial.println(myChar);
  }
  u8g2.setColorIndex(1);
  u8g2.clearBuffer();
  u8g2.drawXBMP(PlayerX, PlayerY, 16, 8, Player);
  if (spawn) spawntimer++; // Give X and Y cords after a constant timer
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
  for (int k = 0; k < maxenemies; k++) { // Spawn enemies and give the ability to shoot
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
    if (enemies[k].active && ((PlayerY <= enemies[k].Y + 5 && PlayerX >= enemies[k].X - 7 && PlayerX <= enemies[k].X + 15) || (PlayerY <= enemies[k].deathorb.Y + 4 && PlayerY >= enemies[k].deathorb.Y - 4 && PlayerX >= enemies[k].deathorb.X - 13 && PlayerX <= enemies[k].deathorb.X + 2))) DIE();
  }
  while (BulletCounter < Bulletslimit) { // check if enemy is hit
    if (bullets[BulletCounter].Y >= 5) bullets[BulletCounter].DrawBullet();
    for (int j = 0; j < maxenemies; j++) {
      if (bullets[BulletCounter].Y < enemies[j].Y + 8 && bullets[BulletCounter].Y > enemies[j].Y - 3 && bullets[BulletCounter].X > enemies[j].X - 3 && bullets[BulletCounter].X < enemies[j].X + 15 && enemies[j].active) {
        enemies[j].active = false;
        hit = true;
      }
    }
    bullets[BulletCounter].Y -= 4;
    last = BulletCounter;
    BulletCounter++;
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
  if (allinactive) { // tell wave number after enemies are cleared
    spawn = true;
    u8g2.setFont(u8g2_font_courB10_tr);
    String waveStr = "WAVE:" + String(wavenum);
    u8g2.drawStr(35, 25, waveStr.c_str());
  }
  BulletCounter = 0;
  if (Bulletslimit >= sizeof(bullets) / sizeof(bullets[0])) IsBullet = false;
  if (!IsBullet) ammoCount++;
  if (ammoCount == 50) { // spawn player ammo once it runs out
    IsAmmo = true;
    randomValue = random(100) + 10;
  }
  if (IsAmmo) {
    u8g2.drawFilledEllipse(randomValue, AmmoY, 5, 5);
    AmmoY++;
  }
  if (AmmoY > 80) {
    randomValue = random(100) + 10;
    AmmoY = 10;
  }
  if ((PlayerY >= AmmoY - 10 && PlayerY <= AmmoY + 3) && (PlayerX >= randomValue - 15 && PlayerX <= randomValue + 3)) { // give random X and Y cords to ammo when spawning for the first time and when failed to be collected
    IsAmmo = false;
    IsBullet = true;
    AmmoY = 10;
    Bulletslimit = 0;
    ammoCount = 0;
  }
  u8g2.setFont(u8g2_font_5x7_tf);
  String pointsStr = "Points: " + String(points); // print current points accumulated by the player
  u8g2.drawStr(42, 7, pointsStr.c_str());
  u8g2.sendBuffer();
  delay(50);
}