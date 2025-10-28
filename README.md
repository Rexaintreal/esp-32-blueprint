# ESP32 Game Console
An interactive game console built on ESP32 with a 0.96 inch OLED display! Features 4 classic games with sound effects, visual feedback, and smooth gameplay.

## Overview

So basically this project turns an ESP32 into a handheld gaming device. It includes a startup animation with blinking eyes, a menu system, and four fully playable games - Snake, Pong, Race (Lane Changer), and Dino. The console uses physical buttons for controls, an active buzzer for sound effects, and a Green LED for visual feedback.

## Hardware Components

- ESP32 Development Board
- 0.96 inch OLED Display (128x64, I2C)
- 5 Push Buttons (UP, DOWN, LEFT, RIGHT, SELECT)
- Buzzer (Active or Passive) (I used active)
- Green LED (Or any other LED)
- 220Ω Resistor (for LED)
- Breadboard
- Jumper Wires (Male-to-Male, Male-to-Female)

## Pin Connections

### OLED Display (I2C)
```
VCC → 3.3V
GND → GND
SDA → GPIO 21
SCL → GPIO 22
```

### Buttons (connect other end to GND Diagonal to the GPIO Pin)
```
UP     → GPIO 32
DOWN   → GPIO 33
LEFT   → GPIO 25
RIGHT  → GPIO 26
SELECT → GPIO 27
```
### Buzzer
```
(+) → GPIO 18
(-) → GND
```

### LED (with 220Ω resistor so it dosent blow up or smth)
```
(+) → GPIO 19
(-) → 220Ω → GND
```

## Required Libraries 
(hOPING YOU ALREADDY HAVE THE ESP THING SET UP IN THE IDE If not check below)
Install these libraries through the Arduino IDE Library Manager:

1. **Wire** (Built-in with Arduino IDE)
2. **Adafruit GFX Library** by Adafruit
3. **Adafruit SSD1306** by Adafruit


### Installation Steps

1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "Adafruit GFX" and install it
4. Search for "Adafruit SSD1306" and install it
5. Restart Arduino IDE

## How to Upload

1. Install the ESP32 board support in Arduino IDE:
   - Go to File → Preferences
   - Add `https://dl.espressif.com/dl/package_esp32_index.json` to Additional Board Manager URLs
   - Go to Tools → Board → Boards Manager
   - Search for "ESP32" and install "ESP32 by Espressif Systems"

2. Connect your ESP32 to your computer via USB (micro USB most prob)

3. Select the correct board and port:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module 
   - Tools → Port → (Select your COM port)

4. Open the `espgame.ino` file

5. Click Upload button (→)

6. Wait for the code to compile and upload

## Games

### 1. Snake
Classic snake game ykwim snake eats the fruit avoid walls thats it! uses all the movement 4 buttons 

### 2. Pong
Keep the ball alive by hitting it with your paddle (Single player only uses 2 buttons UP and Down only)

### 3. Racer
Dodge incoming obstacles by switching lanes. Speed increases as your score goes up. Use LEFT and RIGHT buttons to change lanes.

### 4. Dino
Classic DINO Game uses UP and Down button only for obstacles

## Features

- Startup animation with animated eyes and loading bar (Welcome screen)
- Menu system with visual selection
- Sound effects for all game actions
- LED feedback synchronized with game events
- Increasing difficulty as score goes up
- Game over screen with final score
- Button debouncing for smooth controls

## Common Issues

**Display not working:**
- Check I2C connections (SDA and SCL)
- Verify display address is 0x3C (change in code if different)
- Make sure VCC is connected to 3.3V not 5V
- Make sure Ground is connected properly (it gets dim a bit ig)

**Buttons not responding:**
- PRESS HARDER YEAH THE 4 LEGGED BUTTONS NEED SOME FORCE
- Ensure buttons are connected between GPIO pins and GND
- Check if INPUT_PULLUP is enabled in code
- Try different buttons to isolate hardware issues

**No sound from buzzer:**
- Check buzzer polarity
- Try swaping buzzer wires
- Some buzzers need external oscillation circuit

**LED not lighting:**
- Verify 220Ω resistor is in series with LED (IMP)
- Check LED polarity (longer leg is positive)
- Test LED with external power source

## Building Tips

1. Solder header pins to the OLED display for better connections (mine came with no pins T_T had to solder everything)
2. Use a breadboard to organize all connections (can use male to female cables for the esp if too many connections or small breadboard)
3. Keep wires short to reduce noise and interference
4. Test each component individually before assembling everything 
5. Use heat shrink tubing or electrical tape to secure exposed connections (If any)

## Acknowledgements

**THE INTERNET** - followed many tutorials, articles and webpages while building this.

---

## You may also like...

Some other projects I've built:

- [Libro Voice](https://github.com/Rexaintreal/Libro-Voice) - A PDF to Audio Converter
- [Snippet Vision](https://github.com/Rexaintreal/Snippet-Vision) - A YouTube Video Summarizer
- [Weather App](https://github.com/Rexaintreal/WeatherApp) - A Python Weather Forecast App
- [Python Screenrecorder](https://github.com/Rexaintreal/PythonScreenrecorder) - A Python Screen Recorder
- [Typing Speed Tester](https://github.com/Rexaintreal/TypingSpeedTester) - A Python Typing Speed Tester
- [Movie Recommender](https://github.com/Rexaintreal/Movie-Recommender) - A Python Movie Recommender
- [Password Generator](https://github.com/Rexaintreal/Password-Generator) - A Python Password Generator
- [Object Tales](https://github.com/Rexaintreal/Object-Tales) - A Python Image to Story Generator
- [Finance Manager](https://github.com/Rexaintreal/Finance-Manager) - A Flask WebApp to Monitor Savings
- [Codegram](https://github.com/Rexaintreal/Codegram) - A Social Media Web App for Coders
- [Simple Flask Notes](https://github.com/Rexaintreal/Simple-Flask-Notes) - A Flask Notes App
- [Key5](https://github.com/Rexaintreal/key5) - Python Keylogger
- [Codegram2024](https://github.com/Rexaintreal/Codegram2024) - A Modern Version of Codegram (Update)
- [Cupid](https://github.com/Rexaintreal/cupid) - A Dating Web App for Teenagers
- [Gym Vogue](https://github.com/Rexaintreal/GymVogue/) - Ecommerce Site for Gym Freaks
- [Confessions](https://github.com/Rexaintreal/Confessions) - Anonymous confession platform
- [Syna](https://github.com/Rexaintreal/syna) - A social music web application where users can log in using their Spotify accounts and find their best matches based on shared music preferences
- [Apollo](https://github.com/Rexaintreal/Apollo) - A Minimal Music Player with a Cat Dancing/Bopping to the beats
- [Eros](https://github.com/Rexaintreal/Eros) - A face symmetry analyzer built using Python and OpenCV
- [Notez](https://github.com/Rexaintreal/Notez) - A clean and minimal Android notes application built with Flutter
- [Lynx](https://github.com/Rexaintreal/lynx) - All in one OpenCV image manipulation webapp Built for [Hackberry](https://hackberry.hackclub.com/)
- [DawnDuck](https://github.com/Rexaintreal/Dawnduck) - USB HID automation tool that opens your morning routine apps and websites automatically. Built for [Picoducky](https://picoducky.hackclub.com/)

---

## Author

Built by **Saurabh Tiwari**

- Email: [saurabhtiwari7986@gmail.com](mailto:saurabhtiwari7986@gmail.com)  
- Twitter: [@Saurabhcodes01](https://x.com/Saurabhcodes01)
- Instagram: [@saurabhcodesawfully](https://instagram.com/saurabhcodesawfully)
