#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buttons
#define BTN_UP 32
#define BTN_DOWN 33
#define BTN_LEFT 25
#define BTN_RIGHT 26
#define BTN_SELECT 27
#define BUZZER_PIN 18

// Status LED (Green LED for visual feedback)
#define LED_PIN 19

// Game states
enum GameState { STARTUP, MENU, SNAKE, PONG, CAR, DINO };
GameState currentGame = STARTUP;
int menuSelection = 0;
bool startupDone = false;

// Snake variables
#define MAX_SNAKE 100
int snakeX[MAX_SNAKE], snakeY[MAX_SNAKE];
int snakeLength;
int foodX, foodY;
int dirX, dirY;
unsigned long lastMoveTime = 0;
int snakeScore = 0;
bool gameActive = false;
int snakeSpeed = 120;

// Pong variables
int paddleY;
int ballX, ballY;
int ballDX, ballDY;
int pongScore;
unsigned long pongLastUpdate = 0;
int ballSpeed = 30;

// Car game variables
int playerLane = 1;
int obstacleLane[3] = {-1, -1, -1};
int obstacleY[3] = {-20, -40, -60};
int carScore = 0;
bool carGameActive = false;
unsigned long lastCarMove = 0;
int carSpeed = 80;

// Dino game variables
int dinoY = 44;
int dinoVelocity = 0;
bool isJumping = false;
int obstacleX = 128;
int dinoScore = 0;
unsigned long lastDinoUpdate = 0;
int dinoSpeed = 50;
bool dinoCrouch = false;

// Button debounce
unsigned long lastButtonPress = 0;
#define DEBOUNCE_DELAY 150

// LED control
unsigned long lastLedUpdate = 0;
bool ledState = false;

// Sound functions
void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

void playEat() { playTone(800, 50); digitalWrite(LED_PIN, HIGH); }
void playGameOver() { 
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    playTone(400, 100); delay(100);
    digitalWrite(LED_PIN, LOW);
    playTone(300, 100); delay(100);
  }
  playTone(200, 200);
}
void playJump() { playTone(600, 80); digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW); }
void playScore() { playTone(1000, 60); digitalWrite(LED_PIN, HIGH); delay(60); digitalWrite(LED_PIN, LOW); }
void playMenuMove() { playTone(500, 30); }
void playSelect() { playTone(1200, 100); digitalWrite(LED_PIN, HIGH); delay(100); digitalWrite(LED_PIN, LOW); }

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, LOW);

  randomSeed(analogRead(0));
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  switch (currentGame) {
    case STARTUP:
      if (!startupDone) {
        runStartupAnimation();
        startupDone = true;
      }
      break;
    case MENU: runMenu(); break;
    case SNAKE: runSnake(); break;
    case PONG: runPong(); break;
    case CAR: runCarGame(); break;
    case DINO: runDinoGame(); break;
  }
  delay(10);
}

// ===== STARTUP ANIMATION =====
void runStartupAnimation() {
  // LED blink sequence
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    playTone(523, 100); // C
    delay(100);
    digitalWrite(LED_PIN, LOW);
    playTone(659, 100); // E
    delay(100);
  }
  digitalWrite(LED_PIN, HIGH);
  playTone(784, 150); // G
  delay(150);
  digitalWrite(LED_PIN, LOW);
  
  for (int i = 0; i < 2; i++) {
    drawEyes(64, 28, 0, 0);
    delay(400);
    drawEyes(64, 28, -5, 0);
    delay(300);
    drawEyes(64, 28, 5, 0);
    delay(300);
  }
  
  drawEyes(64, 28, 0, 2);
  delay(200);
  
  for (int j = 0; j < 2; j++) {
    drawEyesClosed(64, 28);
    delay(120);
    drawEyes(64, 28, 0, 2);
    delay(120);
  }
  
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(35, 15);
  display.println("Hi!");
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.println("ESP32 Gaming!");
  display.display();
  playTone(1000, 200);
  delay(1200);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 15);
  display.println("Loading games...");
  for (int i = 0; i < 128; i += 8) {
    display.fillRect(10, 35, i, 8, WHITE);
    display.display();
    digitalWrite(LED_PIN, i % 16 < 8 ? HIGH : LOW);
    delay(30);
  }
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  currentGame = MENU;
}

void drawEyes(int x, int y, int lookX, int lookY) {
  display.clearDisplay();
  display.drawCircle(x - 15, y, 12, WHITE);
  display.fillCircle(x - 15 + lookX, y + lookY, 5, WHITE);
  display.drawCircle(x + 15, y, 12, WHITE);
  display.fillCircle(x + 15 + lookX, y + lookY, 5, WHITE);
  display.display();
}

void drawEyesClosed(int x, int y) {
  display.clearDisplay();
  display.drawLine(x - 27, y, x - 3, y, WHITE);
  display.drawLine(x + 3, y, x + 27, y, WHITE);
  display.display();
}

// ===== MENU =====
void runMenu() {
  // Pulse LED in menu
  if (millis() - lastLedUpdate > 1000) {
    lastLedUpdate = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
  
  if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
    if (digitalRead(BTN_UP) == LOW) {
      menuSelection = (menuSelection - 1 + 4) % 4;
      playMenuMove();
      lastButtonPress = millis();
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      menuSelection = (menuSelection + 1) % 4;
      playMenuMove();
      lastButtonPress = millis();
    }
    if (digitalRead(BTN_SELECT) == LOW) {
      playSelect();
      switch (menuSelection) {
        case 0: initSnake(); currentGame = SNAKE; break;
        case 1: initPong(); currentGame = PONG; break;
        case 2: initCarGame(); currentGame = CAR; break;
        case 3: initDinoGame(); currentGame = DINO; break;
      }
      lastButtonPress = millis();
    }
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 2);
  display.println("* GAMES *");
  display.setTextSize(1);
  display.drawLine(0, 18, 128, 18, WHITE);

  const char* items[] = {">> SNAKE", ">> PONG", ">> RACER", ">> DINO"};
  for (int i = 0; i < 4; i++) {
    int y = 22 + (i * 11);
    display.setCursor(10, y);
    if (menuSelection == i) {
      display.fillRect(8, y - 1, 112, 9, WHITE);
      display.setTextColor(BLACK);
      display.print(items[i]);
      display.setTextColor(WHITE);
    } else {
      display.print(items[i]);
    }
  }
  display.display();
}

// ===== SNAKE =====
void initSnake() {
  digitalWrite(LED_PIN, LOW);
  snakeLength = 4;
  snakeX[0] = 64; snakeY[0] = 32;
  snakeX[1] = 60; snakeY[1] = 32;
  snakeX[2] = 56; snakeY[2] = 32;
  snakeX[3] = 52; snakeY[3] = 32;
  dirX = 1; dirY = 0;
  snakeScore = 0;
  snakeSpeed = 120;
  gameActive = true;
  spawnFood();
}

void spawnFood() {
  foodX = random(1, 31) * 4;
  foodY = random(3, 15) * 4;
}

void runSnake() {
  if (!gameActive) return;
  
  if (millis() - lastButtonPress > 100) {
    if (digitalRead(BTN_UP) == LOW && dirY == 0) { dirX = 0; dirY = -1; lastButtonPress = millis(); }
    else if (digitalRead(BTN_DOWN) == LOW && dirY == 0) { dirX = 0; dirY = 1; lastButtonPress = millis(); }
    else if (digitalRead(BTN_LEFT) == LOW && dirX == 0) { dirX = -1; dirY = 0; lastButtonPress = millis(); }
    else if (digitalRead(BTN_RIGHT) == LOW && dirX == 0) { dirX = 1; dirY = 0; lastButtonPress = millis(); }
    else if (digitalRead(BTN_SELECT) == LOW) { currentGame = MENU; gameActive = false; lastButtonPress = millis(); return; }
  }
  
  if (millis() - lastMoveTime > snakeSpeed) {
    lastMoveTime = millis();
    
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] += dirX * 4;
    snakeY[0] += dirY * 4;
    
    if (snakeX[0] < 0 || snakeX[0] >= 128 || snakeY[0] < 12 || snakeY[0] >= 64) {
      gameOver("SNAKE", snakeScore); return;
    }
    
    for (int i = 1; i < snakeLength; i++) {
      if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
        gameOver("SNAKE", snakeScore); return;
      }
    }
    
    if (abs(snakeX[0] - foodX) < 4 && abs(snakeY[0] - foodY) < 4) {
      if (snakeLength < MAX_SNAKE) {
        snakeLength++; 
        snakeScore += 10;
        playEat();
        delay(60);
        digitalWrite(LED_PIN, LOW);
        spawnFood();
        if (snakeSpeed > 50) snakeSpeed -= 3;
      }
    }
  }
  
  display.clearDisplay();
  display.fillRect(0, 0, 128, 10, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 1);
  display.print("SCORE:"); display.print(snakeScore);
  display.setTextColor(WHITE);
  
  for (int i = 0; i < snakeLength; i++) {
    if (i == 0) display.fillRect(snakeX[i], snakeY[i], 4, 4, WHITE);
    else display.drawRect(snakeX[i], snakeY[i], 3, 3, WHITE);
  }
  
  if (millis() % 400 < 200) display.fillCircle(foodX + 2, foodY + 2, 2, WHITE);
  display.display();
}

// ===== PONG =====
void initPong() {
  digitalWrite(LED_PIN, LOW);
  paddleY = 24;
  ballX = 64; ballY = 32;
  ballDX = 2; ballDY = 2;
  pongScore = 0;
  ballSpeed = 30;
  gameActive = true;
  pongLastUpdate = millis();
}

void runPong() {
  if (!gameActive) return;
  
  if (digitalRead(BTN_UP) == LOW) paddleY -= 4;
  if (digitalRead(BTN_DOWN) == LOW) paddleY += 4;
  if (digitalRead(BTN_SELECT) == LOW && millis() - lastButtonPress > DEBOUNCE_DELAY) { 
    currentGame = MENU; gameActive = false; lastButtonPress = millis(); return; 
  }
  paddleY = constrain(paddleY, 12, 48);
  
  if (millis() - pongLastUpdate > ballSpeed) {
    pongLastUpdate = millis();
    ballX += ballDX;
    ballY += ballDY;
    
    if (ballY <= 12 || ballY >= 62) {
      ballDY = -ballDY;
      playTone(700, 30);
    }
    
    if (ballX <= 10 && ballX >= 6 && ballY >= paddleY && ballY <= paddleY + 16) {
      ballDX = abs(ballDX);
      pongScore++;
      playScore();
      if (ballSpeed > 15) ballSpeed -= 1;
    }
    
    if (ballX >= 126) {
      ballDX = -abs(ballDX);
      playTone(700, 30);
    }
    
    if (ballX <= 0) { 
      gameOver("PONG", pongScore); 
      return; 
    }
  }
  
  display.clearDisplay();
  display.fillRect(0, 0, 128, 10, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 1);
  display.print("SCORE:"); display.print(pongScore);
  display.setTextColor(WHITE);
  
  display.fillRect(4, paddleY, 4, 16, WHITE);
  display.fillCircle(ballX, ballY, 3, WHITE);
  
  for (int i = 12; i < 64; i += 6) {
    display.drawLine(64, i, 64, i + 3, WHITE);
  }
  display.display();
}

// ===== CAR GAME =====
void initCarGame() {
  digitalWrite(LED_PIN, LOW);
  playerLane = 1;
  for (int i = 0; i < 3; i++) {
    obstacleLane[i] = random(0, 3);
    obstacleY[i] = -20 - (i * 25);
  }
  carScore = 0;
  carSpeed = 80;
  carGameActive = true;
  lastCarMove = millis();
}

void runCarGame() {
  if (!carGameActive) return;

  if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
    if (digitalRead(BTN_LEFT) == LOW && playerLane > 0) { 
      playerLane--; 
      playTone(400, 40);
      lastButtonPress = millis(); 
    }
    if (digitalRead(BTN_RIGHT) == LOW && playerLane < 2) { 
      playerLane++; 
      playTone(400, 40);
      lastButtonPress = millis(); 
    }
    if (digitalRead(BTN_SELECT) == LOW) { 
      currentGame = MENU; 
      carGameActive = false; 
      lastButtonPress = millis(); 
      return; 
    }
  }

  if (millis() - lastCarMove > carSpeed) {
    lastCarMove = millis();
    
    for (int i = 0; i < 3; i++) {
      obstacleY[i] += 4;
      
      if (obstacleY[i] > 64) {
        obstacleY[i] = -10;
        obstacleLane[i] = random(0, 3);
        carScore++;
        playScore();
        if (carSpeed > 30) carSpeed -= 2;
      }
      
      if (obstacleY[i] > 40 && obstacleY[i] < 60 && obstacleLane[i] == playerLane) {
        gameOver("RACER", carScore);
        return;
      }
    }
  }

  display.clearDisplay();
  display.fillRect(0, 0, 128, 10, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 1);
  display.print("SCORE:"); display.print(carScore);
  display.setTextColor(WHITE);

  int laneX[3] = {32, 64, 96};
  
  for (int i = 0; i < 3; i++) {
    display.drawLine(laneX[i] - 16, 12, laneX[i] - 16, 64, WHITE);
  }
  display.drawLine(laneX[2] + 16, 12, laneX[2] + 16, 64, WHITE);

  display.fillRect(laneX[playerLane] - 6, 48, 12, 14, WHITE);
  display.drawRect(laneX[playerLane] - 6, 48, 12, 14, BLACK);
  display.fillRect(laneX[playerLane] - 4, 50, 2, 3, BLACK);
  display.fillRect(laneX[playerLane] + 2, 50, 2, 3, BLACK);

  for (int i = 0; i < 3; i++) {
    if (obstacleY[i] > 0 && obstacleY[i] < 64) {
      display.fillRect(laneX[obstacleLane[i]] - 6, obstacleY[i], 12, 14, WHITE);
      display.fillRect(laneX[obstacleLane[i]] - 4, obstacleY[i] + 2, 2, 3, BLACK);
      display.fillRect(laneX[obstacleLane[i]] + 2, obstacleY[i] + 2, 2, 3, BLACK);
    }
  }

  display.display();
}

// ===== DINO GAME =====
void initDinoGame() {
  digitalWrite(LED_PIN, LOW);
  dinoY = 44;
  dinoVelocity = 0;
  isJumping = false;
  obstacleX = 128;
  dinoScore = 0;
  dinoSpeed = 50;
  dinoCrouch = false;
  gameActive = true;
  lastDinoUpdate = millis();
}

void runDinoGame() {
  if (!gameActive) return;

  if (digitalRead(BTN_UP) == LOW && !isJumping && millis() - lastButtonPress > DEBOUNCE_DELAY) {
    isJumping = true;
    dinoVelocity = -8;
    playJump();
    lastButtonPress = millis();
  }
  
  if (digitalRead(BTN_DOWN) == LOW && !isJumping) {
    dinoCrouch = true;
  } else {
    dinoCrouch = false;
  }
  
  if (digitalRead(BTN_SELECT) == LOW && millis() - lastButtonPress > DEBOUNCE_DELAY) {
    currentGame = MENU;
    gameActive = false;
    lastButtonPress = millis();
    return;
  }

  if (millis() - lastDinoUpdate > dinoSpeed) {
    lastDinoUpdate = millis();
    
    if (isJumping) {
      dinoY += dinoVelocity;
      dinoVelocity += 1;
      
      if (dinoY >= 44) {
        dinoY = 44;
        isJumping = false;
        dinoVelocity = 0;
      }
    }
    
    obstacleX -= 4;
    if (obstacleX < -10) {
      obstacleX = 128;
      dinoScore++;
      playScore();
      if (dinoSpeed > 25) dinoSpeed -= 1;
    }
    
    int dinoHeight = dinoCrouch ? 6 : 12;
    int dinoTop = dinoCrouch ? 50 : 44;
    
    if (obstacleX > 10 && obstacleX < 26 && dinoTop + dinoHeight > 48) {
      gameOver("DINO", dinoScore);
      return;
    }
  }

  display.clearDisplay();
  display.fillRect(0, 0, 128, 10, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 1);
  display.print("SCORE:"); display.print(dinoScore);
  display.setTextColor(WHITE);

  display.drawLine(0, 56, 128, 56, WHITE);

  if (dinoCrouch) {
    display.fillRect(12, 50, 12, 6, WHITE);
  } else {
    display.fillRect(14, dinoY, 8, 12, WHITE);
    display.fillRect(12, dinoY + 4, 2, 4, WHITE);
  }

  display.fillRect(obstacleX, 48, 8, 8, WHITE);

  display.display();
}

// ===== GAME OVER =====
void gameOver(String game, int score) {
  gameActive = false;
  carGameActive = false;
  playGameOver();
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(20, 35);
  display.print("Game: ");
  display.print(game);
  display.setCursor(20, 50);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(3000);
  currentGame = MENU;
}
