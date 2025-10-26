#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LEFT_BTN 25
#define RIGHT_BTN 26

// Game variables
int lane = 0;              // 0 = left lane, 1 = right lane
int carY = 50;             // fixed bottom position
int obsY = 0;              // obstacle position
int obsLane = 0;           // which lane obstacle is in
bool gameOver = false;
int score = 0;

void setup() {
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  randomSeed(analogRead(0));
  display.clearDisplay();
  display.display();
}

void loop() {
  if (gameOver) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.print("Game Over");
    display.setTextSize(1);
    display.setCursor(25, 50);
    display.print("Score: ");
    display.print(score);
    display.display();
    delay(2000);
    // Reset
    gameOver = false;
    score = 0;
    obsY = 0;
    obsLane = random(0, 2);
    return;
  }

  // --- Read buttons --- (left and right buttons to change lane)
  bool leftPressed = (digitalRead(LEFT_BTN) == LOW);
  bool rightPressed = (digitalRead(RIGHT_BTN) == LOW);

  if (leftPressed) lane = 0;
  if (rightPressed) lane = 1;

  // --- Move obstacle ---
  obsY += 3;  // falling speed (for 0.96inch display)
  if (obsY > SCREEN_HEIGHT) {
    obsY = 0;
    obsLane = random(0, 2);
    score++;
  }

  // --- Check collision ---
  if (obsY > carY - 10 && obsY < carY + 10 && obsLane == lane) {
    gameOver = true;
  }

  // --- Draw frame ---
  display.clearDisplay();

  // Draw lanes
  display.drawLine(64, 0, 64, 64, SSD1306_WHITE);

  // Draw car (the stationary car we control)
  int carX = (lane == 0) ? 20 : 84;
  display.fillRect(carX, carY, 20, 10, SSD1306_WHITE);

  // Draw obstacle (rectangle for incoming car)
  int obsX = (obsLane == 0) ? 20 : 84;
  display.fillRect(obsX, obsY, 20, 10, SSD1306_WHITE);

  // Score
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.print(score);

  display.display();

  delay(50);
}
