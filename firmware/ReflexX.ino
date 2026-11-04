#include <LiquidCrystal.h>

// ReflexX - Arduino LED Reaction Game
// Target: Arduino Uno / ATmega328P with the wiring documented in README.md.

// LEDs: 1..4 -> D2..D5
const byte ledPins[4] = {2, 3, 4, 5};

// Buttons: 1..4 -> D6..D9.
// Buttons connect the input pin to GND when pressed.
const byte buttonPins[4] = {6, 7, 8, 9};

// LCD: RS, E, D4, D5, D6, D7 -> A0..A5
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

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

  lcd.begin(16, 2);

  // A0-A5 are occupied by the LCD. Do not analogRead() an LCD signal as
  // a random seed. If you have an unused analog input on your board,
  // use it here instead. The fixed seed is functional but repeats patterns.
  randomSeed(analogRead(A6)); // A6 exists on some Nano variants, not classic Uno.
  // For a classic Uno, replace the line above with randomSeed(micros());
  // micros() is not a source of true randomness, but avoids reading an LCD pin.

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
