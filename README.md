# 🎱 Arduino UNO R4 BINGO Number Generator

A BINGO number generator for Arduino UNO R4 WiFi using the built-in 12x8 LED matrix display.

## 🎯 Features

- Generates random numbers from 1 to 75
- **No duplicate numbers** - each number appears exactly once per game
- Visual effects with blinking transitions
- LED warning indicator before number changes
- Scrolling text animations
- Multilingual end message with drawn numbers

## 📟 Hardware Required

- Arduino UNO R4 WiFi (with built-in LED matrix)

## 📚 Libraries Required

- `ArduinoGraphics.h`
- `Arduino_LED_Matrix.h`

Both libraries are included with the Arduino UNO R4 board package.

## 🎮 Game Flow

### Startup Sequence
1. **Welcome scroll**: `.... BINGO .... BINGO .... BINGO ....`
2. **Countdown**: 10 → 9 → 8 → 7 → 6 → 5 → 4 → 3 → 2 → 1 → 0
3. **Starting scroll**: `Starting....`
4. **LED blinks** 5 times
5. **First random number** appears

### Number Display Cycle (13 seconds per number)
| Phase | Duration | Matrix Display | Built-in LED |
|-------|----------|----------------|--------------|
| Fade In | 0-5s | Number blinks slow → fast | Off |
| Steady | 5-8s | Number constant | Off |
| Fade Out | 8-13s | Number blinks fast → slow | Blinking slowly |

### Between Numbers
- **3 seconds** blank display
- LED blinks slowly

### End Sequence (after all 75 numbers)
- Scrolling message repeats forever:
  - `koniec.... end.... Ende.... fin.... fin.... fine.... fim.... konec....`
- Press **RESET** button to restart

## ⏱️ Timing Configuration

You can adjust these constants in the code:

```cpp
const unsigned long DISPLAY_TIME = 13000;    // Number display duration (ms)
const unsigned long BLANK_TIME = 3000;       // Blank period between numbers (ms)
const unsigned long FADE_IN_TIME = 5000;     // Fade in effect duration (ms)
const unsigned long FADE_OUT_TIME = 5000;    // Fade out effect duration (ms)
const unsigned long LED_WARNING_TIME = 5000; // LED warning before change (ms)
```

## 🔧 Installation

1. Install **Arduino IDE** (2.0 or later recommended)
2. Go to **Tools → Board → Boards Manager**
3. Search for **Arduino UNO R4** and install
4. Go to **Sketch → Include Library → Manage Libraries**
5. Search for **ArduinoGraphics** and install
6. Select **Tools → Board → Arduino UNO R4 WiFi**
7. Upload the sketch

## 📁 Files

- `bingo.ino` - Main Arduino sketch

## 🎲 How It Works

The program uses a boolean array to track which numbers (1-75) have been drawn. Each time a new number is needed, it randomly selects from the remaining unused numbers, ensuring no duplicates throughout the game.

The LED matrix uses a custom 3x5 pixel font for displaying digits, with numbers automatically centered on the display.

## 🤖 Vibe Coding

This project was created using **Vibe Coding** - a collaborative development approach with AI assistance (Claude by Anthropic). The code was iteratively developed through natural language conversation, describing desired features and behaviors.

## 📜 License

MIT License - Feel free to use and modify!

## 🙏 Credits

- Built for **Arduino UNO R4 WiFi**
- AI-assisted development with **Claude (Anthropic)**
- Inspired by classic BINGO games

---

*Perfect for family game nights, classroom activities, or any BINGO event!* 🎉
