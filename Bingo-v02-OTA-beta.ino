#include "thingProperties.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Frame buffer for the 12x8 matrix
byte frame[8][12];

// Built-in LED pin
const int LED_PIN = 13;

// Timing constants (in milliseconds)
const unsigned long DISPLAY_TIME = 1000;
const unsigned long BLANK_TIME = 1000;
const unsigned long FADE_IN_TIME = 1000;
const unsigned long FADE_OUT_TIME = 1000;
const unsigned long LED_WARNING_TIME = 1000;

// Array to track used numbers
bool usedNumbers[76];
int drawnNumbers[75];
int drawCount = 0;
int numbersRemaining = 75;
int currentNumber = 0;
bool gameFinished = false;

// End message with numbers
String endMessage;

// Forward declarations
void cloudDelay(unsigned long ms);
void clearFrame();
void drawNumber(int num);
void drawDigit(int digit, int xPos);
void showInitialScroll();
void showStartCountdown();
void showStartingScroll();
void blinkLEDStart();
void buildEndMessage();
void showEndScroll();
void displayNumberWithEffects(int num);
void blankPeriod();
int getNextNumber();

void setup() {
  Serial.begin(9600);
  delay(1500);
  
  matrix.begin();
  pinMode(LED_PIN, OUTPUT);
  randomSeed(analogRead(A0));
  
  // Initialize Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  // Wait for cloud connection
  Serial.print("Connecting to Arduino IoT Cloud");
  while (ArduinoCloud.connected() == 0) {
    ArduinoCloud.update();
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
  }
  Serial.println("\nConnected to Arduino IoT Cloud!");
  
  // Initialize all numbers as unused
  for (int i = 0; i <= 75; i++) {
    usedNumbers[i] = false;
  }
  
  // Show initial BINGO scroll
  showInitialScroll();
  
  delay(1000);
  showStartCountdown();
  showStartingScroll();
  blinkLEDStart();
  
  currentNumber = getNextNumber();
}

void loop() {
  ArduinoCloud.update();
  
  if (gameFinished) {
    showEndScroll();
    return;
  }
  
  displayNumberWithEffects(currentNumber);
  blankPeriod();
  
  if (numbersRemaining == 0) {
    buildEndMessage();
    gameFinished = true;
    return;
  }
  
  currentNumber = getNextNumber();
}

void cloudDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    ArduinoCloud.update();
    delay(10);
  }
}

int getNextNumber() {
  if (numbersRemaining == 0) return 0;
  
  int num;
  do {
    num = random(1, 76);
  } while (usedNumbers[num]);
  
  usedNumbers[num] = true;
  numbersRemaining--;
  drawnNumbers[drawCount] = num;
  drawCount++;
  
  return num;
}

void showInitialScroll() {
  cloudDelay(500);
  
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(".... BINGO .... BINGO .... BINGO ....");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  cloudDelay(12000);
}

void showStartCountdown() {
  for (int count = 10; count >= 0; count--) {
    clearFrame();
    drawNumber(count);
    matrix.renderBitmap(frame, 8, 12);
    cloudDelay(1000);
  }
}

void showStartingScroll() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println("Starting....");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  cloudDelay(4000);
}

void blinkLEDStart() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    cloudDelay(100);
    digitalWrite(LED_PIN, LOW);
    cloudDelay(100);
  }
}

void buildEndMessage() {
  endMessage = "koniec.... end.... Ende.... fin.... fine.... fim.... konec.... ";
  
  for (int i = 0; i < 75; i++) {
    endMessage += String(drawnNumbers[i]);
    if (i < 74) {
      endMessage += " - ";
    }
  }
  
  endMessage += " ....";
}

void showEndScroll() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(60);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(endMessage);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  cloudDelay(45000);
}

void displayNumberWithEffects(int num) {
  unsigned long startTime = millis();
  unsigned long elapsed;
  
  while ((elapsed = millis() - startTime) < DISPLAY_TIME) {
    ArduinoCloud.update();
    
    unsigned long remaining = DISPLAY_TIME - elapsed;
    bool showNumber = true;
    
    if (elapsed < FADE_IN_TIME) {
      float progress = (float)elapsed / FADE_IN_TIME;
      int blinkInterval = 500 - (int)(progress * 450);
      showNumber = ((millis() / blinkInterval) % 2) == 0;
    } else if (remaining <= FADE_OUT_TIME) {
      float progress = (float)(FADE_OUT_TIME - remaining) / FADE_OUT_TIME;
      int blinkInterval = 50 + (int)(progress * 450);
      showNumber = ((millis() / blinkInterval) % 2) == 0;
    } else {
      showNumber = true;
    }
    
    if (remaining <= LED_WARNING_TIME) {
      bool ledState = ((millis() / 400) % 2) == 0;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
    
    clearFrame();
    if (showNumber) {
      drawNumber(num);
    }
    matrix.renderBitmap(frame, 8, 12);
    
    delay(10);
  }
  
  digitalWrite(LED_PIN, LOW);
}

void blankPeriod() {
  unsigned long startTime = millis();
  
  clearFrame();
  matrix.renderBitmap(frame, 8, 12);
  
  while ((millis() - startTime) < BLANK_TIME) {
    ArduinoCloud.update();
    
    bool ledState = ((millis() / 400) % 2) == 0;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    delay(10);
  }
  
  digitalWrite(LED_PIN, LOW);
}

void clearFrame() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 12; x++) {
      frame[y][x] = 0;
    }
  }
}

void drawNumber(int num) {
  String numStr = String(num);
  int totalWidth = numStr.length() * 4 - 1;
  int xOffset = (12 - totalWidth) / 2;
  
  for (unsigned int i = 0; i < numStr.length(); i++) {
    drawDigit(numStr.charAt(i) - '0', xOffset + i * 4);
  }
}

void drawDigit(int digit, int xPos) {
  const byte digits[10][5] = {
    {0b111, 0b101, 0b101, 0b101, 0b111},
    {0b010, 0b110, 0b010, 0b010, 0b111},
    {0b111, 0b001, 0b111, 0b100, 0b111},
    {0b111, 0b001, 0b111, 0b001, 0b111},
    {0b101, 0b101, 0b111, 0b001, 0b001},
    {0b111, 0b100, 0b111, 0b001, 0b111},
    {0b111, 0b100, 0b111, 0b101, 0b111},
    {0b111, 0b001, 0b001, 0b001, 0b001},
    {0b111, 0b101, 0b111, 0b101, 0b111},
    {0b111, 0b101, 0b111, 0b001, 0b111}
  };
  
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 3; x++) {
      if (digits[digit][y] & (0b100 >> x)) {
        int px = xPos + x;
        int py = y + 1;
        if (px >= 0 && px < 12 && py >= 0 && py < 8) {
          frame[py][px] = 1;
        }
      }
    }
  }
}
