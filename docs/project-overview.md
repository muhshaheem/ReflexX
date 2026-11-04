# Project overview

## Objective
Create a compact interactive game that trains visual attention and button-to-target matching using four LED/button pairs.

## Functional flow
1. Idle screen prompts the player to press any button.
2. A random LED is selected and illuminated.
3. The LCD identifies the target LED and displays the score.
4. The firmware scans the four active-low button inputs.
5. A matching button increments the score; a mismatch ends the game.
6. The player can restart using any button.

## Scope
The current version is a target-matching game. It is not a precision reaction-time instrument because it does not timestamp LED activation and button response. A future version can use `millis()` to measure response latency and report milliseconds.

## Electrical notes
- Each LED requires its own current-limiting resistor.
- Buttons use the ATmega328P internal pull-up resistors and should connect to GND when pressed.
- The 16×2 LCD uses an I²C backpack connected to the Arduino Uno SDA (A4) and SCL (A5) pins.
- Keep all module grounds common.
- Install the hd44780 library by Bill Perry; the firmware uses its hd44780_I2Cexp interface to detect supported backpacks.
- Keep the LCD and Arduino grounds common; use a backpack/LCD module compatible with the Arduino Uno's 5 V I²C logic.
- Do not use A4/A5 for other functions while the LCD is connected to the I²C bus.

## Validation record
The repository includes the supplied wiring assignment and a cleaned firmware structure. The author should record the actual Arduino board revision, LCD module, Arduino IDE version, compile result, and hardware test result here after bench verification.
