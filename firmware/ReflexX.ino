#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// ReflexX - Arduino LED Reaction Game
// Target: Arduino Uno / ATmega328P with the wiring documented in README.md.

// LEDs: 1..4 -> D2..D5
const byte ledPins[4] = {2, 3, 4, 5};

// Buttons: 1..4 -> D6..D9.
// Buttons connect the input pin to GND when pressed.
const byte buttonPins[4] = {6, 7, 8, 9};

// 16x2 LCD connected through an I2C backpack (PCF8574/MCP23008).
// The library attempts to detect the backpack address and pin mapping.
hd44780_I2Cexp lcd;

unsigned int score = 0;
int currentLED = -1;
bool gameRunning = false;

void turnOffLEDs();
void waitForStart();
void startGame();
int waitForButton();
void gameOver();

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Initialize the I2C LCD backpack (auto-detect address/pin mapping).
  // hd44780::begin() returns 0 on success.
  const int lcdStatus = lcd.begin(16, 2);
  if (lcdStatus != 0) {
    Serial.print(F("LCD initialization failed. Status: "));
    Serial.println(lcdStatus);
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(250);
    }
  }

  // Arduino Uno I2C pins: SDA=A4, SCL=A5.
  // Keep these pins dedicated to the LCD's I2C bus.

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED REACTION");
  lcd.setCursor(0, 1);
  lcd.print("Press any key");

  Serial.println(F("ReflexX - ready"));
  waitForStart();
  startGame();
}

void loop() {
  if (!gameRunning) {
    return;
  }

  // One loop iteration is one complete game round.
  turnOffLEDs();
  currentLED = random(0, 4);
  digitalWrite(ledPins[currentLED], HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Press LED ");
  lcd.print(currentLED + 1);

  Serial.println(F("------------------------------"));
  Serial.print(F("Target LED: "));
  Serial.println(currentLED + 1);
  Serial.print(F("Score: "));
  Serial.println(score);
  Serial.println(F("Waiting for button..."));

  const int pressedButton = waitForButton();
  digitalWrite(ledPins[currentLED], LOW);

  Serial.print(F("Button pressed: "));
  Serial.println(pressedButton + 1);

  if (pressedButton == currentLED) {
    score++;

    Serial.println(F("Result: CORRECT"));
    Serial.print(F("New score: "));
    Serial.println(score);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CORRECT!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);

    delay(2000); // Loop then automatically begins the next round.
  } else {
    Serial.println(F("Result: WRONG"));
    Serial.print(F("Expected button: "));
    Serial.println(currentLED + 1);
    gameOver();
  }
}

void waitForStart() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(40); // Basic debounce.
        if (digitalRead(buttonPins[i]) == LOW) {
          while (digitalRead(buttonPins[i]) == LOW) {
            delay(5);
          }
          return;
        }
      }
    }
  }
}

void startGame() {
  randomSeed(micros()); // Timing-derived seed; not cryptographically random.
  score = 0;
  currentLED = -1;
  gameRunning = true;
  turnOffLEDs();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME START!");
  lcd.setCursor(0, 1);
  lcd.print("Get ready...");

  Serial.println();
  Serial.println(F("========== GAME START =========="));
  delay(1000);

  // Do not treat a button held during startup as a game response.
  for (byte i = 0; i < 4; i++) {
    while (digitalRead(buttonPins[i]) == LOW) {
      delay(5);
    }
  }
  turnOffLEDs();
}

int waitForButton() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(30); // Debounce.
        if (digitalRead(buttonPins[i]) == LOW) {
          while (digitalRead(buttonPins[i]) == LOW) {
            delay(5);
          }
          return i;
        }
      }
    }
  }
}

void turnOffLEDs() {
  for (byte i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void gameOver() {
  gameRunning = false;
  turnOffLEDs();

  Serial.println();
  Serial.println(F("========== GAME OVER =========="));
  Serial.print(F("Final score: "));
  Serial.println(score);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press any key");
  lcd.setCursor(0, 1);
  lcd.print("to restart");

  waitForStart();
  startGame();
}
