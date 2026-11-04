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
- LCD is wired in 4-bit mode to A0-A5.
- Keep all module grounds common.
- Do not use A0-A5 as random-seed analog inputs while they are connected to the LCD.

## Validation record
The repository includes the supplied wiring assignment and a cleaned firmware structure. The author should record the actual Arduino board revision, LCD module, Arduino IDE version, compile result, and hardware test result here after bench verification.
