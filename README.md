# ReflexX — Arduino LED Reaction Game

ReflexX is a small, interactive reaction game built around an Arduino Uno, four LEDs, four push buttons, and a 16×2 HD44780-compatible LCD. The Arduino randomly lights one LED. The player must press the matching button; a correct response increases the score, while an incorrect response ends the round and displays the final score.

> **Project status:** Initial open-source release. The firmware below follows the supplied circuit pin mapping. Hardware operation and timing have not been independently tested by the repository maintainer; verify the wiring and upload procedure on your exact board.

## Features

- Four randomly selected LED targets.
- Four corresponding push-button inputs.
- 16×2 LCD score and prompt display.
- Score increases after each correct response.
- Wrong input ends the game and allows restart.
- Serial Monitor diagnostics at 9600 baud.
- One round per `loop()` iteration; no duplicated nested round logic.

## Hardware

| Part | Quantity | Notes |
|---|---:|---|
| Arduino Uno (ATmega328P) | 1 | Board assumed by the pin map in this project |
| LEDs | 4 | Use a current-limiting resistor (typically 220–330 Ω) per LED |
| Push buttons | 4 | Connected to ground when pressed; internal pull-ups are enabled |
| 16×2 character LCD | 1 | HD44780-compatible, 4-bit mode |
| 10 kΩ potentiometer | 1 | LCD contrast (VO) |
| Breadboard and jumper wires | As needed | Common ground required |
| USB cable | 1 | Programming and power |

## Pin connections

### LEDs

| LED | Arduino pin |
|---|---:|
| LED 1 | D2 |
| LED 2 | D3 |
| LED 3 | D4 |
| LED 4 | D5 |

Connect each LED in series with its own resistor. In the firmware, the selected LED output is driven HIGH. A typical connection is Arduino pin → resistor → LED anode, LED cathode → GND. Confirm polarity before powering.

### Push buttons

| Button | Arduino pin |
|---|---:|
| Button 1 | D6 |
| Button 2 | D7 |
| Button 3 | D8 |
| Button 4 | D9 |

Each button connects its assigned input pin to GND when pressed. Firmware configures these pins as `INPUT_PULLUP`, so the unpressed state reads HIGH and the pressed state reads LOW. Do not connect these button pins to 5 V in this arrangement.

### 16×2 LCD (4-bit mode)

The sketch uses the Arduino `LiquidCrystal` library with this constructor:

```cpp
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
```

| LCD pin/signal | Arduino connection |
|---|---:|
| RS | A0 |
| E | A1 |
| D4 | A2 |
| D5 | A3 |
| D6 | A4 |
| D7 | A5 |
| RW | GND |
| VSS | GND |
| VDD | 5 V |
| VO | Wiper of 10 kΩ contrast potentiometer |
| LED+ / A | 5 V through suitable backlight resistor if the LCD module does not include one |
| LED− / K | GND |

**Important:** A0–A5 are all occupied by the LCD. Do not call `analogRead(A5)` for a random seed: A5 is being used as the LCD D7 signal. The firmware seeds the pseudorandom generator from an unconnected analog input only if one is available; the default wiring shown uses all six analog pins. For better variation, connect a genuinely unused analog input on a board with one available, or use a floating analog input only during startup. Never expect a cryptographically random sequence from Arduino's `random()`.

## Software requirements

- Arduino Uno / ATmega328P-compatible board.
- Arduino IDE. The sketch uses the built-in `LiquidCrystal` library and Arduino core functions; no extra library installation is required.
- USB data cable (some cables are charge-only).
- Correct USB-serial driver for clone boards, if applicable (for example CH340/CH341).

## Arduino IDE: compile and upload

1. Install and open Arduino IDE.
2. Open `firmware/ReflexX.ino`.
3. Connect the Arduino using a known-good USB **data** cable.
4. Select **Tools → Board → Arduino AVR Boards → Arduino Uno** (or select the exact board you actually have).
5. Select **Tools → Port** and choose the port that appears when the board is connected.
6. Click **Verify** (✓) to compile.
7. Click **Upload** (→) to compile and transfer the sketch.
8. Open **Tools → Serial Monitor**, set the baud rate to **9600**, and reset the board if needed.

Do not select a board merely because its name looks similar. The board selection controls the compile target and upload protocol. If using a Nano or a clone, check its processor/bootloader option under **Tools → Processor** where that menu exists.

### Arduino IDE version note

This project does not require an old Arduino IDE version. Use a current stable Arduino IDE 2.x or a compatible Arduino IDE 1.8.x installation. If a particular older board package, USB driver, or computer setup works only with your previously installed IDE version, you may use that version, but IDE age is not a general bootloader requirement. Keep the selected board package and upload settings consistent.

## Bootloader: what it does and when to burn it

The bootloader is a small program stored on the microcontroller that allows a compatible sketch to be uploaded over the serial/USB interface. A normal Arduino Uno that already has a working bootloader **does not need its bootloader burned before every sketch upload**. Uploading a sketch and burning/replacing a bootloader are different operations.

For a normal upload, use **Upload** in Arduino IDE. Do not choose **Burn Bootloader** as a routine step before uploading code.

You may need to burn/reinstall the bootloader only when:
- The ATmega328P is blank or has no bootloader.
- The bootloader is corrupted or has been overwritten.
- The board cannot accept serial uploads despite verified power, cable, port, drivers, and board settings.
- You are programming a bare ATmega328P for the first time.

### Burning the Uno bootloader with a second Arduino (ISP)

This is a recovery/setup procedure and requires a second working Arduino Uno (the programmer), jumper wires, and the target Uno (the board receiving the bootloader).

1. Connect the programmer Uno to the computer and upload **File → Examples → 11.ArduinoISP → ArduinoISP** to the programmer.
2. Wire programmer to target:
   - Programmer D10 → Target RESET
   - Programmer D11 → Target D11 (MOSI)
   - Programmer D12 → Target D12 (MISO)
   - Programmer D13 → Target D13 (SCK)
   - Programmer 5V → Target 5V (only if the target is intended to be powered from the programmer; avoid conflicting power supplies)
   - Programmer GND → Target GND
3. Keep the target connected as ISP. In Arduino IDE select **Tools → Board → Arduino Uno** for the target.
4. Select the programmer port (the programmer Uno's COM/serial port).
5. Select **Tools → Programmer → Arduino as ISP** (not “ArduinoISP”).
6. Choose **Tools → Burn Bootloader**.
7. Wait for the success message, then disconnect the ISP wiring.
8. Reconnect the target normally by USB, select its serial port, and upload `ReflexX.ino`.

**Caution:** The ISP wiring above is for a classic Uno using the ICSP signals mapped to D10–D13. Other boards, processors, and programmer hardware may require different wiring and settings. Do not connect two active 5 V supplies together. Verify your board's voltage and pinout before wiring.

## Troubleshooting uploads

| Symptom | Checks |
|---|---|
| No port appears | Try a known data cable, another USB port, and the correct USB driver for the board's USB-serial chip. |
| `avrdude: ser_open(): can't open device` | Close Serial Monitor and other serial applications; check port selection and OS permissions. |
| `stk500_recv()` / programmer not responding | Check board and processor selection, selected port, cable, reset/bootloader health, and any shields/wiring on serial pins D0/D1. |
| Upload starts but fails | Disconnect external circuits temporarily, especially anything connected to D0/RX and D1/TX; try manual reset near upload start. |
| Compile error for `LiquidCrystal.h` | Select a standard Arduino AVR board package; `LiquidCrystal` is normally included with the Arduino AVR core. |
| LCD backlight works but no text | Adjust the contrast potentiometer; verify RS/E/D4–D7 wiring and RW-to-GND. |
| Random LED behavior seems repetitive | Arduino `random()` is pseudorandom. Seed from an unused, electrically noisy analog input at startup; do not read from an LCD-connected pin. |

## How to play

1. Power the board.
2. The LCD shows **LED REACTION / Press any key**.
3. Press any button to start.
4. When an LED lights, press the button with the same number.
5. Each correct response adds one point. The next round starts after a 2-second score display.
6. Pressing a wrong button displays **GAME OVER** and the final score.
7. Press any button to restart.

## Firmware behavior

The main `loop()` performs exactly one round:
1. Turn all LEDs off.
2. Pick a target LED and turn it on.
3. Display the score and target.
4. Wait for one debounced button press and release.
5. Turn the target LED off.
6. Increment the score on a match; otherwise call `gameOver()`.

The next round is started naturally by Arduino's next call to `loop()`. The code intentionally does not manually run another round inside the correct-answer branch.

## Repository layout

```text
ReflexX/
├── README.md
├── LICENSE
├── .gitignore
└── firmware/
    └── ReflexX.ino
```

## Known limitations and next improvements

- This is a simple button-matching game, not a calibrated reaction-time measurement system. It does not currently calculate reaction time in milliseconds.
- Button scanning is blocking and uses simple delay-based debounce.
- Randomness is pseudorandom and depends on seeding.
- Add a buzzer, difficulty progression, best-score persistence (EEPROM), non-blocking timing, or reaction-time measurement in future revisions.

## Contributing

Issues and pull requests are welcome. Include your board model, Arduino IDE version, operating system, wiring changes, and the complete upload error when reporting a problem.

## License

Released under the MIT License. See [LICENSE](LICENSE).
