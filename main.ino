#include <LedControl.h>
#include "pitches.h"
#include <EEPROM.h>
#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>

const int dinPin = 12;
const int clockPin = 8;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;
const int swPin = 9;

const int highScore1 = 10;
const int highScore2 = 20;
const int highScore3 = 30;
const int highScoreName1 = 40;
const int highScoreName2 = 50;
const int highScoreName3 = 60;
const int difficulty = 70;

const int RS = 7;
const int enable = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
const int noMatrix = 2;

LedControl lc = LedControl(dinPin, clockPin, loadPin, noMatrix); //DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

unsigned int readHighScore1 = 0;
String nameHighScore1;
unsigned int readHighScore2 = 0;
String nameHighScore2;
unsigned int readHighScore3 = 0;
String nameHighScore3;

unsigned int startingLevel = 0;
int setStartLevel = 0;
const int maxLevel = 2;

int setNameState = 0;
int letterIndex = 0;
const int noLettersName = 3;
int nameLetters[noLettersName] = {0, 0, 0};
char defaultName[noLettersName];
String currentName = "AAA";
const char noLetters = 26;
const char letters[noLetters] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int letterIndexName = 0;
bool enterMenu = 0;

byte matrixBrightness = 2;
int startingChar = 16;
int endingChar = 0;
int startingChar1 = 16;
int endingChar1 = 0;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

bool buttonState = 1;
bool lastButtonState = 1;
unsigned long lastDebounce = 0;
unsigned int debounceDelay = 80;
unsigned long scrollTime = 0;
int swValue = 0;
int xValue = 0;
int yValue = 0;

const int minThreshold = 200;
const int maxThreshold = 600;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

const int welcomeMsgTime = 500;
int menuPos = 0;
int menuState = 0;
const int menuStates = 4;

const int menuStatesAbout = 3;
int menuStateAbout = 0;

const int menuStatesSettings = 3;
int menuStateSettings = 0;

const int menuStatesHighscore = 4;
int menuStateHighscore = 0;
unsigned long score = 0;

bool xState = 0;
bool yState = 0;

int menuPosAbout = 0;

int blockState = 0;
int newBlockState = 0;
byte blockIndex = 0;

int nextBlock = 1;
int randomBlock = 1;

unsigned long lastDrop;
unsigned long dropInterval = 1000;

unsigned long lastInput;
const unsigned long inputDelay = 120;

const int matrixSize = 8;
int movBlock[matrixSize + 4][matrixSize];
int matrix[matrixSize][matrixSize];

struct rotate {
  int x, y;
};

byte sad[8]= {0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C};

byte fullBlock[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

int blocks[][4][matrixSize] = {
  {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 2, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 2, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 2, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 2, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 2, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 2, 1, 0, 0},
  },
};



void(* resetFunc) (void) = 0;

///----------------------------------

void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}

void writeIntE(int address, int number)
{ 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

void writeStringE(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
}

int readIntE(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

String readStringE(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}

String scrollLeft1(String string) {
  String result;
  String strProcess = "                " + string + "                ";
  result = strProcess.substring(startingChar, endingChar);
  startingChar++;
  endingChar++;
  if (startingChar > strProcess.length()) {
    startingChar = 16;
    endingChar = 0;
  }
  return result;
}

String scrollLeft2(String string) {
  String result;
  String strProcess = "                " + string + "                ";
  result = strProcess.substring(startingChar1, endingChar1);
  startingChar1++;
  endingChar1++;
  if (startingChar1 > strProcess.length()) {
    startingChar1 = 16;
    endingChar1 = 0;
  }
  return result;
}

int changeMenuPos(int currentPos, int xValue, int menuStates) {
  int positionUpdated = currentPos;
  if (xValue > maxThreshold && !xState) {
    positionUpdated = currentPos - 1;
    xState = 1;
    tone(buzzerPin, 100, 100);
  }
  if (xValue < minThreshold && !xState) {
    positionUpdated = currentPos + 1;
    xState = 1;
    tone(buzzerPin, 100, 100);
  }
  if (xValue > minThreshold && xValue < maxThreshold && xState) {
    xState = 0;
  }
  if (positionUpdated == -1) {
    positionUpdated = 0;
  }
  else if (positionUpdated == menuStates) {
    positionUpdated = menuStates - 1;
  }
  return positionUpdated;
}

void setName(int xVal, int yVal) {
  if (yVal > maxThreshold && !yState) {
    if (letterIndex == noLettersName - 1) {
      letterIndex = 0;
    }
    else {
      letterIndex++;
    }
    yState = 1;
  }
  if (yVal < minThreshold && !yState) {
    if (letterIndex == 0) {
      letterIndex = noLettersName - 1;
    }
    else {
      letterIndex--;
    }
    yState = 1;
  }
  if (yVal < maxThreshold && yVal > minThreshold && yState) {
    yState = 0;
  }
  if (xVal < minThreshold && !xState) {
    if (nameLetters[letterIndex] == noLetters - 1) {
      nameLetters[letterIndex] = 0;
    }
    else {
      nameLetters[letterIndex]++;
    }
    xState = 1;
  }
  else if (xVal > maxThreshold && !xState) {
    if (nameLetters[letterIndex] == 0) {
      nameLetters[letterIndex] = noLetters - 1;
    }
    else {
      nameLetters[letterIndex]--;
    }
    xState = 1;
  }
  else if (xVal < maxThreshold && xVal > minThreshold && xState) {
    xState = 0;
  }
  for (int i = 0; i < noLettersName; i++) {
    defaultName[i] = letters[nameLetters[i]];
  }
  currentName = String(defaultName);
}

void setStartingLevel(int xVal) {
  if (xVal < minThreshold && !xState) {
    if (startingLevel == 0) {
      startingLevel = maxLevel;
    }
    else {
      startingLevel--;
    }
    xState = 1;
  }
  else if (xVal > maxThreshold && !xState) {
    if (startingLevel == maxLevel) {
      startingLevel = 0;
    }
    else {
      startingLevel++;
    }
    xState = 1;
  }
  else if (xVal < maxThreshold && xVal > minThreshold && xState) {
    xState = 0;
  }
  writeIntE(difficulty, startingLevel);
}

//-------------- game logic ---------------

void displayMatrix() {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, movBlock[i + 4][j] | matrix[i][j]);
    }
  }
}

int gameOver() {
  for (int j = 0; j < matrixSize; j++) {
    if (matrix[0][j]) {
      return 1;
    }
  }
  return 0;
}

void joystickMove(int yValue, int xValue) {
  if (yValue < minThreshold) {
    moveBlock(-1, 0); //left
  }
  else if (yValue > maxThreshold) {
    moveBlock(1, 0);  //right
  }
  else if (xValue > maxThreshold) {
    rotateBlock();
  }
  else if (xValue < minThreshold) {
    lastDrop -= dropInterval;
  }
}

void updateState() {
  if (collisionBottom()) {
    updateMatrix();
    waitBlock();
  }
  moveBlock(0, -1);
  deleteCompletedRows();
}

void renderLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.setCursor(6, 0);
  if (startingLevel == 0) {
    lcd.print("Easy");
  }
  else if (startingLevel == 1) {
    lcd.print(" Medium");
  }
  else if (startingLevel == 2) {
    lcd.print(" Hard");
  }

  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.setCursor(6, 1);
  lcd.print(score);
}

void deleteCompletedRows() {
  int rowsUpdated = 0;
  for (int i = 0; i < matrixSize; i++) {
    int completedRow = 1;
    for (int j = 0; j < matrixSize; j++) {
      if (!matrix[i][j]) {
        completedRow = 0;
      }
    }

    if (completedRow) {
      rowsUpdated++;
      Serial.println(i);
      for (int j = 0; j < matrixSize; j++) {
        matrix[i][j] = 0;
      }
      for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
          Serial.print(matrix[i][j]);
        }
        Serial.println();
      }
      int newMatrix[matrixSize][matrixSize];
      for (int k = i; k >= 0; k--) {
        for (int l = 0; l < matrixSize; l++) {
          if (k - 1 >= 0) {
            newMatrix[k][l] = matrix[k - 1][l];
          }
        }
      }

      for (int k = i + 1; k < matrixSize + 4; k++) {
        for (int l = 0; l < matrixSize; l++) {
          newMatrix[k][l] = matrix[k][l];
        }
      }

      for (int k = 0; k < matrixSize; k++) {
        for (int l = 0; l < matrixSize; l++) {
          matrix[k][l] = newMatrix[k][l];
        }
      }
    }
  }
  if (rowsUpdated) {
    if (rowsUpdated == 1) {
      score += 50 * (startingLevel + 1);
    }
    else if (rowsUpdated == 2) {
      score += 150 * (startingLevel + 1);
    }
    else if (rowsUpdated == 3) {
      score += 400 * (startingLevel + 1);
    }
    else if (rowsUpdated == 4) {
      score += 1000 * (startingLevel + 1);
    }
    else if (rowsUpdated == 5) {
      score += 3000 * (startingLevel + 1);
    }
  }
}

void waitBlock() {
  randomBlock = random(sizeof(blocks) / sizeof(blocks[0]) - 1);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      movBlock[i][j] = blocks[randomBlock][i][j];
    }
  }
  //randomBlock = random(sizeof(blocks) / sizeof(blocks[0]) - 1);
  //randomBlock = nextBlock;
  //nextBlock = random(sizeof(blocks) / sizeof(blocks[0]) - 1);
}

int collisionBottom() {
  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (movBlock[i][j] && i == 11 || 
         (movBlock[i][j] && i >= 4 && 
            i + 1 < matrixSize + 4 && 
            matrix[i - 4 + 1][j])) {
        return 1;
      }
    }
  }
  return 0;
}

void updateMatrix() {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      matrix[i][j] = matrix[i][j] || movBlock[i + 4][j];
      movBlock[i + 4][j] = 0;
    }
  }
}

void rotateBlock() {
  rotate rotatePoint = {-1, -1};

  int newMovBlock[matrixSize + 4][matrixSize];

  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      newMovBlock[i][j] = 0;
      if (movBlock[i][j] == 2) {
        rotatePoint.y = i;
        rotatePoint.x = j;
      }
    }
  }
  
  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (movBlock[i][j] == 1) {
        rotate newRotatePoint = {rotatePoint.x + rotatePoint.y - i, j - rotatePoint.x + rotatePoint.y};
        if (newRotatePoint.x >= matrixSize || newRotatePoint.x < 0 || newRotatePoint.y >= matrixSize + 4 || newRotatePoint.y < 0 || (newRotatePoint.y >= 4 && matrix[newRotatePoint.y - 4][newRotatePoint.x])) {
          return;
        }

        newMovBlock[newRotatePoint.y][newRotatePoint.x] = 1;
      } else if (movBlock[i][j] == 2) {
        newMovBlock[i][j] = 2;
      }
    }
  }
  //Serial.println("newMovBlock:");
  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      //Serial.print(newMovBlock[i][j]);
    }
    //Serial.println();
  }

  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      movBlock[i][j] = newMovBlock[i][j];
    }
  }
}

int crossBoundries(int xValid, int x) {
  for (int i = 0; i < matrixSize + 4 && xValid; i++) {  //crossing the boundries
    for (int j = 0; j < matrixSize && xValid; j++) {
      if (movBlock[i][j]) {
        if (j + x >= matrixSize || j + x < 0) {
          xValid = 0;
        }
      }
    }
  }
  return xValid;
}

void moveBlock(int x, int y) {
  y = -y;
  int xValid = 1;
  int newMovBlock[matrixSize + 4][matrixSize];

  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      newMovBlock[i][j] = 0;
    }
  }
  
  xValid = crossBoundries(xValid, x);  //crossing the boundries

  for (int i = 0; i < matrixSize + 4; i++) {  //move the block
    for (int j = 0; j < matrixSize; j++) {
      if (movBlock[i][j]) {
        if (i + y < matrixSize + 4 && i + y >= 0) {
          if (xValid) {
            if (i >= 4 && matrix[i - 4 + y][j + x]) {
              return;
            }
            newMovBlock[i + y][j + x] = movBlock[i][j];
          } else {
            newMovBlock[i + y][j] = movBlock[i][j];
          }
        }
      }
    }
  }

  for (int i = 0; i < matrixSize + 4; i++) {
    for (int j = 0; j < matrixSize; j++) {
      movBlock[i][j] = newMovBlock[i][j];
    }
  }
  //Serial.println("movBlock:");
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++) {
      //Serial.print(movBlock[i][j]);
    }
    //Serial.println();
  }
}

void updateHighScore() {
  if (score > readHighScore1) {
    readHighScore3 = readHighScore2;
    nameHighScore3 = nameHighScore2;
    readHighScore2 = readHighScore1;
    nameHighScore2 = nameHighScore1;
    readHighScore1 = score;
    nameHighScore1 = currentName;
  }
  else if (score > readHighScore2) {
    readHighScore3 = readHighScore2;
    nameHighScore3 = nameHighScore2;
    readHighScore2 = score;
    nameHighScore2 = currentName;
  }
  else if (score > readHighScore3) {
    readHighScore3 = score;
    nameHighScore3 = currentName;
  }

  writeIntE(highScore1, readHighScore1);
  writeIntE(highScore2, readHighScore2);
  writeIntE(highScore3, readHighScore3);

  writeStringE(highScoreName1, nameHighScore1);
  writeStringE(highScoreName2, nameHighScore2);
  writeStringE(highScoreName3, nameHighScore3);
}

void setup() {
  randomSeed(analogRead(A5));
  readHighScore1 = readIntE(highScore1);
  readHighScore2 = readIntE(highScore2);
  readHighScore3 = readIntE(highScore3);
  nameHighScore1 = readStringE(highScoreName1);
  nameHighScore2 = readStringE(highScoreName2);
  nameHighScore3 = readStringE(highScoreName3);
  pinMode(buzzerPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  for (int i = 0; i < noMatrix; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, matrixBrightness);
    lc.clearDisplay(i);
  }
  lcd.createChar(0, fullBlock);
  waitBlock();
}

void loop() {
  //tone(buzzerPin, 1000, 500);
  xValue = analogRead(xPin);
  swValue = digitalRead(swPin);

  if (swValue != lastButtonState) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > debounceDelay) {
    if (swValue != buttonState) {
      buttonState = swValue;
    }
  }
  lastButtonState = swValue;

  if (millis() < welcomeMsgTime) {
    lcd.setCursor(3, 0);
    lcd.print("Welcome to");
    lcd.setCursor(5, 1);
    lcd.print("Tetris");
  }
  else if(menuState == 0) {
    switch (menuPos) {
      case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->StartGame");
        lcd.setCursor(2, 1);
        lcd.print("HighScore");
        menuPos = changeMenuPos(menuPos, xValue, menuStates);
        if (buttonState == 0) {
          menuState = 1;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;
        
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->HighScore");
        lcd.setCursor(2, 1);
        lcd.print("Settings");
        menuPos = changeMenuPos(menuPos, xValue, menuStates);
        if (buttonState == 0) {
          menuState = 2;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;
        
        case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->Settings");
        lcd.setCursor(2, 1);
        lcd.print("About");
        menuPos = changeMenuPos(menuPos, xValue, menuStates);
        if (buttonState == 0) {
          menuState = 3;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;

        case 3:
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("->About");
        menuPos = changeMenuPos(menuPos, xValue, menuStates);
        if (buttonState == 0) {
          menuState = 4;
          buttonState = 1;
          lastButtonState = 1;
        }
        
        delay(50);
        break;
    }
  }
  else if (menuState == 1) {  //start game
    if (startingLevel == 0) {
      dropInterval = 1000;
    }
    else if (startingLevel == 1) {
      dropInterval = 600;
    }
    else if (startingLevel == 2) {
      dropInterval = 300;
    }
    yValue = analogRead(yPin);
    xValue = analogRead(xPin);
    if (gameOver()) {
      tone(buzzerPin, 500, 400);
      delay(400);
      tone(buzzerPin, 300, 400);
      delay(400);
      tone(buzzerPin, 100, 400);
      updateHighScore();
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Game Over!");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.setCursor(6, 1);
      lcd.print(score);
      printByte(sad);
      delay(9000);
      resetFunc();
    }
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastInput > inputDelay) {
      joystickMove(yValue, xValue);
      lastInput = currentMillis;
    }
    if (currentMillis - lastDrop > dropInterval) {
      updateState();
      renderLcd();
      lastDrop = currentMillis;
    }
    displayMatrix();
  }
  else if (menuState == 2) {  //highscore
    readHighScore1 = readIntE(highScore1);
    nameHighScore1 = readStringE(highScoreName1);
    readHighScore2 = readIntE(highScore2);
    nameHighScore2 = readStringE(highScoreName2);
    readHighScore3 = readIntE(highScore3);
    nameHighScore3 = readStringE(highScoreName3);
    switch (menuStateHighscore) {
      case 0:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Highscore");
        lcd.setCursor(0, 1);
        lcd.print("->");
        lcd.setCursor(2, 1);
        lcd.print(nameHighScore1);
        lcd.print(":");
        lcd.setCursor(6, 1);
        lcd.print(readHighScore1);
        menuStateHighscore = changeMenuPos(menuStateHighscore, xValue, menuStatesHighscore);
        delay(50);
        break;
        
      case 1:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Highscore");
        lcd.setCursor(0, 1);
        lcd.print("->");
        lcd.setCursor(2, 1);
        lcd.print(nameHighScore2);
        lcd.print(":");
        lcd.setCursor(6, 1);
        lcd.print(readHighScore2);
        menuStateHighscore = changeMenuPos(menuStateHighscore, xValue, menuStatesHighscore);
        delay(50);
        break;

      case 2:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Highscore");
        lcd.setCursor(0, 1);
        lcd.print("->");
        lcd.setCursor(2, 1);
        lcd.print(nameHighScore3);
        lcd.print(":");
        lcd.setCursor(6, 1);
        lcd.print(readHighScore3);
        menuStateHighscore = changeMenuPos(menuStateHighscore, xValue, menuStatesHighscore);
        delay(50);
        break;

        case 3:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Highscore");
        lcd.setCursor(0, 1);
        lcd.print("->Exit");
        menuStateHighscore = changeMenuPos(menuStateHighscore, xValue, menuStatesHighscore);
        if (buttonState == 0) {
          menuState = 0;
          menuStateHighscore = 0;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;
    }
  }
  else if (menuState == 3) {  //settings
    switch (menuStateSettings) {
      case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->Nickname:");
        lcd.setCursor(11, 0);
        lcd.print(defaultName);
        Serial.println(defaultName[3]);
        lcd.setCursor(2, 1);
        lcd.print("Level:");
        lcd.setCursor(8, 1);
        startingLevel = readIntE(difficulty);
        if (startingLevel == 0) {
          lcd.print("Easy");
        }
        else if (startingLevel == 1) {
          lcd.print("Medium");
        }
        else if (startingLevel == 2) {
          lcd.print("Hard");
        }
        if (!setNameState && !setStartLevel) {
          menuStateSettings = changeMenuPos(menuStateSettings, xValue, menuStatesSettings);
        }
        if (buttonState == 0 && !setNameState) {
          setNameState = 1;
          buttonState = 1;
          lastButtonState = 1;
          letterIndex = 0;
        }
        delay(50);
        break;

        case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->Level:");
        lcd.setCursor(8, 0);  
        startingLevel = readIntE(difficulty);
        if (startingLevel == 0) {
          lcd.print("Easy");
        }
        else if (startingLevel == 1) {
          lcd.print("Medium");
        }
        else if (startingLevel == 2) {
          lcd.print("Hard");
        }
        lcd.setCursor(2, 1);
        lcd.print("Exit");
        if (!setNameState && !setStartLevel) {
          menuStateSettings = changeMenuPos(menuStateSettings, xValue, menuStatesSettings);
        }
        if (buttonState == 0 && !setStartLevel) {
          setStartLevel = 1;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;

        case 2:
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Level:");
        lcd.setCursor(8, 0);
        if (startingLevel == 0) {
          lcd.print("Easy");
        }
        else if (startingLevel == 1) {
          lcd.print("Medium");
        }
        else if (startingLevel == 2) {
          lcd.print("Hard");
        }
        lcd.setCursor(0, 1);
        lcd.print("->Exit");
        menuStateSettings = changeMenuPos(menuStateSettings, xValue, menuStatesSettings);
        if (buttonState == 0) {
          menuState = 0;
          menuStateSettings = 0;
          buttonState = 1;
          lastButtonState = 1;
        }
        delay(50);
        break;
    }
    if (setNameState) {
      yValue = analogRead(yPin);
      setName(xValue, yValue);
      if (buttonState == 0) {
        setNameState = 0;
        buttonState = 1;
        lastButtonState = 1;
      }
    }
    if (setStartLevel) {
      setStartingLevel(xValue);
      if (buttonState == 0) {
        setStartLevel = 0;
        buttonState = 1;
        lastButtonState = 1;
      }
    }
  }
  else if (menuState == 4) {  //about
    if (enterMenu == 0) {
      startingChar = 16;
      endingChar = 0;
      startingChar1 = 16;
      endingChar1 = 0;
      enterMenu = 1;
    }
    switch (menuStateAbout) {
      case 0:
      menuStateAbout = changeMenuPos(menuStateAbout, xValue, menuStatesAbout);
      if (millis() - scrollTime > 350) {
        scrollTime = millis();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(scrollLeft1("Tetris by Catalin Rotaru")); 
      }
      lcd.setCursor(0,1);
      lcd.print("https://github.com/Coakaze/Matrix-Game");
      break;

      case 1:
      menuStateAbout = changeMenuPos(menuStateAbout, xValue, menuStatesAbout);
      lcd.setCursor(0,0);
      lcd.print("Tetris by Catalin Rotaru");
      if (millis() - scrollTime > 350) {
        scrollTime = millis();
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(scrollLeft2("https://github.com/Coakaze/Matrix-Game")); 
      }
      break;

      case 2:
      menuStateAbout = changeMenuPos(menuStateAbout, xValue, menuStatesAbout);
      if (millis() - scrollTime > 350) {
        scrollTime = millis();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(scrollLeft2("https://github.com/Coakaze/Matrix-Game")); 
      }
      lcd.setCursor(0, 1);
      lcd.print("->Exit");
      if (buttonState == 0) {
          menuState = 0;
          menuStateAbout = 0;
          enterMenu = 0;
          buttonState = 1;
          lastButtonState = 1;
        }
      break;
    }
  }
}
