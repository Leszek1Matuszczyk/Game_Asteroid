#include <TFT_eSPI.h>
#include "Free_Fonts.h"
#include <SPI.h>

TFT_eSPI lcd = TFT_eSPI();

//Define player structure

struct play {
  float pX;
  float pY;
  float pdX;
  float pdY;
  float pdA;
  float wingLX;
  float wingLY;
  float wingRX;
  float wingRY;
  float cockpitX;
  float cockpitY;
  float speedX;
  float speedY;
};
//Define missile structure

struct Bx {
  bool cast;
  float pX;
  float pY;
  float vX;
  float vY;
  float vA;
  byte distance;
};

//Define Rock structure

struct rock {
  float pX;
  float pY;
  float vX;
  float vY;
  float vA;
  float topoX[16];
  float topoY[16];
  byte height[16];
  byte R;
};


#define tile 25
#define PI 3.1415926535
#define maxCast 20
#define maxStones 4

//global Variables

play player = { 100, 100 };
Bx missile[maxCast] = { 0 };
long int permission = 0;
byte casteMissiles = 0;
rock stone[maxStones] = { { 210, 40 }, { 30, 40 }, { 30, 280 },{210,280} };
int score=150000;
long time=0;
bool playerInit=1;
bool end = 0;

// Function to steer the player and draw the player

int PlayerFunc(play gamer) {
  
  lcd.fillCircle(gamer.pX, gamer.pY, 2, TFT_BLACK);
  lcd.drawLine(gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, TFT_BLACK);
  lcd.drawLine(gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, TFT_BLACK);
  lcd.drawLine(gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, TFT_BLACK);
  lcd.fillCircle(gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, 1, TFT_BLACK);
  lcd.fillCircle(gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, 1, TFT_BLACK);
  lcd.fillCircle(gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, 1, TFT_BLACK);

  // Player steering logic
 
  if (digitalRead(WIO_5S_LEFT) == 0||playerInit==1) {
    gamer.pdA += 0.1;
    if (gamer.pdA > 2 * PI) {
      gamer.pdA -= 2 * PI;
    }
    gamer.pdX = sin(gamer.pdA) * 0.02;
    gamer.pdY = cos(gamer.pdA) * 0.02;
    gamer.cockpitX = sin(gamer.pdA) * 10;
    gamer.cockpitY = cos(gamer.pdA) * 10;
    gamer.wingLX = sin(gamer.pdA - 2.5) * 10;
    gamer.wingLY = cos(gamer.pdA - 2.5) * 10;
    gamer.wingRX = sin(gamer.pdA + 2.5) * 10;
    gamer.wingRY = cos(gamer.pdA + 2.5) * 10;
  }
  if (digitalRead(WIO_5S_RIGHT) == 0) {
    gamer.pdA -= 0.1;
    if (gamer.pdA < 0) {
      gamer.pdA += 2 * PI;
    }   
    gamer.pdX = sin(gamer.pdA) * 0.02;
    gamer.pdY = cos(gamer.pdA) * 0.02;
    gamer.cockpitX = sin(gamer.pdA) * 10;
    gamer.cockpitY = cos(gamer.pdA) * 10;
    gamer.wingLX = sin(gamer.pdA - 2.5) * 10;
    gamer.wingLY = cos(gamer.pdA - 2.5) * 10;
    gamer.wingRX = sin(gamer.pdA + 2.5) * 10;
    gamer.wingRY = cos(gamer.pdA + 2.5) * 10;
  }
  // if (digitalRead(WIO_5S_DOWN) == 0) { unused input
  //   gamer.pX -=gamer.pdX;
  //   gamer.pY -=gamer.pdY;
  // }
  if (digitalRead(WIO_5S_UP) == 0) {
    gamer.speedX += gamer.pdX;
    gamer.speedY += gamer.pdY;
  }

  // Update player position
  
  gamer.pX += gamer.speedX;
  gamer.pY += gamer.speedY;
  if (gamer.pX >= 241) {
    gamer.pX = 0;
  }
  if (gamer.pX <= -1) {
    gamer.pX = 240;
  }
  if (gamer.pY >= 321) {
    gamer.pY = 0;
  }
  if (gamer.pY <= -1) {
    gamer.pY = 320;
  }

 // Drawing the updated player
  
  player = gamer;
  lcd.drawLine(gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, TFT_WHITE);
  lcd.drawLine(gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, TFT_WHITE);
  lcd.drawLine(gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, TFT_WHITE);


  lcd.fillCircle(gamer.pX, gamer.pY, 2, TFT_WHITE);
  lcd.fillCircle(gamer.cockpitX + gamer.pX, gamer.cockpitY + gamer.pY, 1, TFT_BLUE);
  lcd.fillCircle(gamer.wingLX + gamer.pX, gamer.wingLY + gamer.pY, 1, TFT_RED);
  lcd.fillCircle(gamer.wingRX + gamer.pX, gamer.wingRY + gamer.pY, 1, TFT_GREEN);
}

// Function to handle firing missiles

void fire() {

  if (digitalRead(WIO_KEY_A) == 0 && millis() > permission + 100) {
    missile[casteMissiles].cast = 1;
    missile[casteMissiles].pX = player.pX;
    missile[casteMissiles].pY = player.pY;
    missile[casteMissiles].vA = player.pdA;
    Serial.println(missile[casteMissiles].cast);
    casteMissiles++;
    if (casteMissiles >= maxCast) {
      missile[casteMissiles].cast = 0;
      casteMissiles = 0;
    }
    permission = millis();
  }


  for (int i = 0; i < maxCast; i++) {
    if (missile[i].cast == 1) {
      lcd.fillCircle(missile[i].pX, missile[i].pY, 1, TFT_BLACK);
      missile[i].vX = sin(missile[i].vA) * 2;
      missile[i].vY = cos(missile[i].vA) * 2;
      missile[i].pX += missile[i].vX;
      missile[i].pY += missile[i].vY;
      missile[i].distance++;

      lcd.fillCircle(missile[i].pX, missile[i].pY, 1, TFT_WHITE);
      if (missile[i].distance == 70) {
        missile[i].distance = 0;
        missile[i].cast = 0;
        lcd.fillCircle(missile[i].pX, missile[i].pY, 1, TFT_BLACK);
      }
    }
    if (missile[i].pX >= 241) {
      missile[i].pX = 0;
    }
    if (missile[i].pX <= -1) {
      missile[i].pX = 240;
    }
    if (missile[i].pY >= 321) {
      missile[i].pY = 0;
    }
    if (missile[i].pY <= -1) {
      missile[i].pY = 320;
    }
  }
}
// Function to draw and animate enemy movement (stones)

void enemy() {
  for (int j = 0; j < maxStones; j++) {
    for (int i = 0; i < 16; i++) {

      stone[j].topoX[i] = sin(stone[j].vA + 0.39269908169 * i) * stone[j].height[i];
      stone[j].topoY[i] = cos(stone[j].vA + 0.39269908169 * i) * stone[j].height[i];
    }
    float X = sin(stone[j].vA) * stone[j].height[0];
    float Y = cos(stone[j].vA) * stone[j].height[0];
    for (int i = 0; i < 16; i++) {
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY, TFT_BLACK);
      lcd.drawLine(X + stone[j].pX + 240, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX + 240, stone[j].topoY[i] + stone[j].pY, TFT_BLACK);
      lcd.drawLine(X + stone[j].pX - 240, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX - 240, stone[j].topoY[i] + stone[j].pY, TFT_BLACK);
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY + 320, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY + 320, TFT_BLACK);
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY - 320, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY - 320, TFT_BLACK);
      X = stone[j].topoX[i];
      Y = stone[j].topoY[i];
    }

    lcd.drawLine(sin(stone[j].vA) * stone[j].height[0] + stone[j].pX, cos(stone[j].vA) * stone[j].height[0] + stone[j].pY, X + stone[j].pX, Y + stone[j].pY, TFT_BLACK);

    stone[j].vA += 0.1;
    stone[j].pX = stone[j].pX + stone[j].vX;
    stone[j].pY = stone[j].pY + stone[j].vY;



    if (stone[j].pX >= 241) {
      stone[j].pX = 0;
    }
    if (stone[j].pX <= -1) {
      stone[j].pX = 240;
    }
    if (stone[j].pY >= 321) {
      stone[j].pY = 0;
    }
    if (stone[j].pY <= -1) {
      stone[j].pY = 320;
    }



    for (int i = 0; i < 16; i++) {

      stone[j].topoX[i] = sin(stone[j].vA + 0.39269908169 * i) * stone[j].height[i];
      stone[j].topoY[i] = cos(stone[j].vA + 0.39269908169 * i) * stone[j].height[i];
    }
    X = sin(stone[j].vA) * stone[j].height[0];
    Y = cos(stone[j].vA) * stone[j].height[0];
    for (int i = 0; i < 16; i++) {
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY, TFT_WHITE);
      lcd.drawLine(X + stone[j].pX + 240, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX + 240, stone[j].topoY[i] + stone[j].pY, TFT_WHITE);
      lcd.drawLine(X + stone[j].pX - 240, Y + stone[j].pY, stone[j].topoX[i] + stone[j].pX - 240, stone[j].topoY[i] + stone[j].pY, TFT_WHITE);
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY + 320, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY + 320, TFT_WHITE);
      lcd.drawLine(X + stone[j].pX, Y + stone[j].pY - 320, stone[j].topoX[i] + stone[j].pX, stone[j].topoY[i] + stone[j].pY - 320, TFT_WHITE);
      X = stone[j].topoX[i];
      Y = stone[j].topoY[i];
    }

    lcd.drawLine(sin(stone[j].vA) * stone[j].height[0] + stone[j].pX, cos(stone[j].vA) * stone[j].height[0] + stone[j].pY, X + stone[j].pX, Y + stone[j].pY, TFT_WHITE);
  }
}

// Function to detect collisions between player, stones, and missiles

void collision() {
  for (int j = 0; j < maxStones; j++) {
    float pX, pY, pD, mX, mY, mD;
    if (player.pX > stone[j].pX) {
      pX = player.pX - stone[j].pX;
    } else {
      pX = stone[j].pX - player.pX;
    }
    if (player.pY > stone[j].pY) {
      pY = player.pY - stone[j].pY;
    } else {
      pY = stone[j].pY - player.pY;
    }

    pD = sqrt(pX * pX + pY * pY);
    if (pD < 20) {
      end = 1;
    }
    //Serial.println(pD);
    for (int i = 0; i < maxCast; i++) {
      if (missile[i].cast == 1) {
        if (missile[i].pX > stone[j].pX) {
          mX = missile[i].pX - stone[j].pX;
        } else {
          mX = stone[j].pX - missile[i].pX;
        }
        if (missile[i].pY > stone[j].pY) {
          mY = missile[i].pY - stone[j].pY;
        } else {
          mY = stone[j].pY - missile[i].pY;
        }
        mD = sqrt(mX * mX + mY * mY);
        Serial.println(mD);
        if (mD < 20) {
          stone[j].vX += missile[i].vX / 3;
          stone[j].vY += missile[i].vY / 3;
          missile[i].cast = 0;
          lcd.fillCircle(missile[i].pX, missile[i].pY, 1, TFT_BLACK);
        }
      }
    }
  }
}
void setup() {
  lcd.init();
  lcd.fillScreen(TFT_BLACK);

  //Welcome screen
  
  lcd.setRotation(3);
  lcd.fillScreen(TFT_BLACK);
  lcd.setCursor(2, 12);
  lcd.setTextColor(TFT_WHITE);
  lcd.setFreeFont(FF1);
  lcd.print("Simple old school style game \ncreated using Arduino Ide.\n      Have fun playing!");
  lcd.setCursor(40, 120);
  lcd.setTextColor(TFT_WHITE);
  lcd.setFreeFont(FF4);
  lcd.print("Asteroid");
  lcd.setCursor(25, 132);
  lcd.setFreeFont(FF1);
  lcd.print("Leszek Enterteinment Corp.");
  delay(15000);
  lcd.setRotation(2);
  lcd.fillScreen(TFT_BLACK);
  Serial.begin(9600);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  
  for (int i = 0; i < maxStones; i++) {
    for (int j = 0; j < 16; j++) {
      stone[i].height[j] = random(15, 20);
    }
  }
  for (int i = 0; i < maxStones; i++) {
    stone[i].vX = random(-100, 100) / 100.0;
    stone[i].vY = random(-100, 100) / 100.0;
  }
  

}

void loop() {
while(millis()>time+10)
{

  PlayerFunc(player);
playerInit=0;

  fire();
  enemy();
  collision();
score= millis()-15000;
//Game Over screen
  while (end == 1) {
    
    lcd.setRotation(3);
  lcd.fillScreen(TFT_BLACK);
  
  
  lcd.setCursor(40, 120);
  lcd.setTextColor(TFT_WHITE);
  lcd.setFreeFont(FF4);
  lcd.print("Game Over");
  lcd.setCursor(60, 140);
  lcd.setFreeFont(FF1);
  lcd.print("your Score: ");
  lcd.print(score);
  while(1){}
  }

  time=millis();
}

}
