#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;
const int swPin = 9;

const int RS = 7;
const int enable = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

unsigned int startingLevel = 0;
int setStartLevel = 0;
const int maxLevel = 3;

int setNameState = 0;
int letterIndex = 0;
const int noLettersName = 3;
int nameLetters[noLettersName] = {0, 0, 0};
char defaultName[noLettersName];
const char noLetters = 26;
const char letters[noLetters] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int letterIndexName = 0;

byte matrixBrightness = 2;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

bool buttonState = 1;
bool lastButtonState = 1;
unsigned long lastDebounce = 0;
unsigned int debounceDelay = 80;
int swValue = 0;
int xValue = 0;
int yValue = 0;

const int minThreshold = 200;
const int maxThreshold = 600;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

const byte matrixSize = 8;
bool matrixChanged = true;

const int welcomeMsgTime = 4000;
int menuPos = 0;
int menuState = 0;
const int menuStates = 4;

const int menuStatesAbout = 2;
int menuStateAbout = 0;

const int menuStatesSettings = 3;
int menuStateSettings = 0;

bool xState = 0;
bool yState = 0;

int menuPosAbout = 0;

bool matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

//coada de schimbari cu matrice de bytes
byte matrixByte[matrixSize] = {
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
};

void generateFood() {
  // lastFoodPos = currentPos;
  // newFoodPos = random(ceva);
  // matrix[lastFoodPos] = 0;
  // matrix[newFoodPos] = 1;
  matrixChanged = true;
}

void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  
  xLastPos = xPos;
  yLastPos = yPos;
  if (xValue < minThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    } 
    else {
      xPos = 0;
    }
  }

  if (xValue > maxThreshold) {
    if (xPos > 0) {
      xPos--;
    }
    else {
      xPos = matrixSize - 1;
    }
  }

  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } 
    else {
      yPos = 0;
    }
  }

  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    }
    else {
      yPos = matrixSize - 1;
    }
  }
  if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
  
}

int changeMenuPos(int currentPos, int xValue, int menuStates) {
  int positionUpdated = currentPos;
  if (xValue > maxThreshold && !xState) {
    positionUpdated = currentPos - 1;
    xState = 1;
  }
  if (xValue < minThreshold && !xState) {
    positionUpdated = currentPos + 1;
    xState = 1;
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
}

void setup() {
  pinMode(swPin, INPUT_PULLUP);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  matrix[xPos][yPos] = 1;
}

void loop() {
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
  Serial.println(buttonState);

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
        delay(50);
        break;
        
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->HighScore");
        lcd.setCursor(2, 1);
        lcd.print("Settings");
        menuPos = changeMenuPos(menuPos, xValue, menuStates);
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
    
  }
  else if (menuState == 2) {  //highscore
    
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
        lcd.print(startingLevel);
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
        lcd.print(startingLevel);
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
        lcd.print(startingLevel);
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
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Tetris by Catalin Rotaru");
    lcd.setCursor(1,1);
    lcd.print("Github link: https://github.com/Coakaze/Matrix-Game"); 
    delay(50);
  }

//  updateMatrix();
//   if (millis() - lastMoved > moveInterval) {
//   // game logic
//     updatePositions();
//     lastMoved = millis();
//   }
//  
//   if (matrixChanged == true) {
//    //matrix display logic
//    updateMatrix();
//    matrixChanged = false;
//   }
}
