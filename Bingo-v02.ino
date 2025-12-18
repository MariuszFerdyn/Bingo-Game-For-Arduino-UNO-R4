#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Frame buffer for the 12x8 matrix
byte frame[8][12];

// Built-in LED pin
const int LED_PIN = 13;

// Timing constants (in milliseconds)
const unsigned long DISPLAY_TIME = 13000;    // 13 seconds display
const unsigned long BLANK_TIME = 3000;       // 3 seconds blank
const unsigned long FADE_IN_TIME = 5000;     // 5 seconds fade in (slow to fast blink)
const unsigned long FADE_OUT_TIME = 5000;    // 5 seconds fade out (fast to slow blink)
const unsigned long LED_WARNING_TIME = 5000; // 5 seconds LED warning before end


// Array to track used numbers
bool usedNumbers[76];  // Index 1-75 used, index 0 unused
int drawnNumbers[75];  // Store numbers in order they were drawn
int drawCount = 0;     // How many numbers have been drawn
int numbersRemaining = 75;
int currentNumber = 0;
bool gameFinished = false;

// End message with numbers
String endMessage;

void setup() {
  matrix.begin();
  pinMode(LED_PIN, OUTPUT);
  randomSeed(analogRead(A0));
  
  // Initialize all numbers as unused
  for (int i = 0; i <= 75; i++) {
    usedNumbers[i] = false;
  }
  
  // Show initial BINGO scroll
  showInitialScroll();
  
  // Short pause before countdown
  delay(1000);
  
  // Show countdown 10 to 0
  showStartCountdown();
  
  // Show "Starting....." scroll
  showStartingScroll();
  
  // Short pause before first digit
  blinkLEDStart();
  
  // Get first number
  currentNumber = getNextNumber();
}

void loop() {
  // If game is finished, show end scroll forever
  if (gameFinished) {
    showEndScroll();
    return;
  }
  
  // Phase 1: Display number for 13 seconds with effects
  displayNumberWithEffects(currentNumber);
  
  // Phase 2: Blank period for 3 seconds with slow LED blinking
  blankPeriod();
  
  // Check if all numbers have been used
  if (numbersRemaining == 0) {
    // Build end message with all drawn numbers
    buildEndMessage();
    
    // Set game as finished - will loop end scroll forever
    gameFinished = true;
    return;
  }
  
  // Generate next unique number
  currentNumber = getNextNumber();
}

int getNextNumber() {
  if (numbersRemaining == 0) return 0;
  
  // Generate random number that hasn't been used
  int num;
  do {
    num = random(1, 76);
  } while (usedNumbers[num]);
  
  usedNumbers[num] = true;
  numbersRemaining--;
  
  // Store the number in draw order
  drawnNumbers[drawCount] = num;
  drawCount++;
  
  return num;
}

void showInitialScroll() {
  // Wait a moment for matrix to initialize
  delay(500);
  
  // Scroll BINGO message 3 times
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(".... BINGO .... BINGO .... BINGO ....");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  // Wait for scroll to complete
  delay(12000);
}

void showStartCountdown() {
  // Display countdown from 10 to 0
  for (int count = 10; count >= 0; count--) {
    clearFrame();
    drawNumber(count);
    matrix.renderBitmap(frame, 8, 12);
    delay(1000);  // 1 second per number
  }
}

void showStartingScroll() {
  // Scroll "Starting...."
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println("Starting....");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  // Wait for scroll to complete
  delay(4000);
}

void blinkLEDStart() {
  // Quick LED blinks before first digit (1 second total)
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void buildEndMessage() {
  // Start with multilingual end message
  endMessage = "koniec.... end.... Ende.... fin.... fine.... fim.... Numbers drawn: ";
  
  // Add all drawn numbers in order
  for (int i = 0; i < 75; i++) {
    endMessage += String(drawnNumbers[i]);
    if (i < 74) {
      endMessage += " - ";
    }
  }
  
  endMessage += " ....";
}

void showEndScroll() {
  // Scroll the end message with numbers
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(60);
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(endMessage);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  
  // Wait for scroll to complete before repeating
  delay(45000);
}

void displayNumberWithEffects(int num) {
  unsigned long startTime = millis();
  unsigned long elapsed;
  
  while ((elapsed = millis() - startTime) < DISPLAY_TIME) {
    
    // Calculate time remaining
    unsigned long remaining = DISPLAY_TIME - elapsed;
    
    // Determine matrix blinking state
    bool showNumber = true;
    
    if (elapsed < FADE_IN_TIME) {
      // First 5 seconds: SLOW to FAST blinking (fade in)
      float progress = (float)elapsed / FADE_IN_TIME;
      int blinkInterval = 500 - (int)(progress * 450);  // 500ms to 50ms
      showNumber = ((millis() / blinkInterval) % 2) == 0;
      
    } else if (remaining <= FADE_OUT_TIME) {
      // Last 5 seconds: FAST to SLOW blinking (fade out)
      float progress = (float)(FADE_OUT_TIME - remaining) / FADE_OUT_TIME;
      int blinkInterval = 50 + (int)(progress * 450);  // 50ms to 500ms
      showNumber = ((millis() / blinkInterval) % 2) == 0;
      
    } else {
      // Middle period: constant display
      showNumber = true;
    }
    
    // Handle warning LED (only last 5 seconds, slow blinking)
    if (remaining <= LED_WARNING_TIME) {
      bool ledState = ((millis() / 400) % 2) == 0;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
    
    // Update display
    if (showNumber) {
      clearFrame();
      drawNumber(num);
      matrix.renderBitmap(frame, 8, 12);
    } else {
      clearFrame();
      matrix.renderBitmap(frame, 8, 12);
    }
    
    delay(10);
  }
  
  digitalWrite(LED_PIN, LOW);
}

void blankPeriod() {
  unsigned long startTime = millis();
  
  // Clear matrix
  clearFrame();
  matrix.renderBitmap(frame, 8, 12);
  
  while ((millis() - startTime) < BLANK_TIME) {
    // Slow LED blinking (400ms interval)
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
    {0b111, 0b101, 0b101, 0b101, 0b111},  // 0
    {0b010, 0b110, 0b010, 0b010, 0b111},  // 1
    {0b111, 0b001, 0b111, 0b100, 0b111},  // 2
    {0b111, 0b001, 0b111, 0b001, 0b111},  // 3
    {0b101, 0b101, 0b111, 0b001, 0b001},  // 4
    {0b111, 0b100, 0b111, 0b001, 0b111},  // 5
    {0b111, 0b100, 0b111, 0b101, 0b111},  // 6
    {0b111, 0b001, 0b001, 0b001, 0b001},  // 7
    {0b111, 0b101, 0b111, 0b101, 0b111},  // 8
    {0b111, 0b101, 0b111, 0b001, 0b111}   // 9
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
