# ReflexX — Arduino LED Reaction Game

ReflexX is a small, interactive reaction game built around an Arduino Uno, four LEDs, four push buttons, and a 16×2 HD44780-compatible LCD. The Arduino randomly lights one LED. The player must press the matching button; a correct response increases the score, while an incorrect response ends the round and displays the final score.

> **Project status:** Initial open-source release. The firmware below follows the supplied circuit pin mapping. Hardware operation and timing have not been independently tested by the repository maintainer; verify the wiring and upload procedure on your exact board.

## Demo video

Watch the ReflexX project demonstration on YouTube:

[▶ Watch the ReflexX demo](https://www.youtube.com/watch?v=VB640a8tsiU)

[![ReflexX video thumbnail](https://img.youtube.com/vi/VB640a8tsiU/0.jpg)](https://www.youtube.com/watch?v=VB640a8tsiU)

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
| 16×2 character LCD with I²C backpack | 1 | HD44780-compatible LCD with a PCF8574- or MCP23008-based I²C backpack |
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

### 16×2 LCD with I²C backpack

The LCD is connected through an I²C backpack (commonly based on a PCF8574 or MCP23008 expander), not through six separate Arduino LCD control/data pins. The firmware uses Bill Perry's `hd44780` library and its `hd44780_I2Cexp` interface. This interface can auto-detect supported backpack addresses and common pin mappings, rather than assuming a fixed address such as `0x27`.

| I²C backpack pin | Arduino Uno connection |
|---|---|
| GND | GND |
| VCC | 5 V (for a 5 V backpack/LCD module) |
| SDA | A4 / SDA |
| SCL | A5 / SCL |

On an Arduino Uno R3, the separate SDA/SCL header pins (if present) are electrically the same I²C signals as A4/A5. Connect SDA to SDA and SCL to SCL. Keep the LCD and Arduino grounds common. The backpack normally provides the LCD contrast trimmer and backlight connections; adjust its small contrast potentiometer if the backlight is on but characters are not visible.

**Important:** Do not connect the LCD's parallel RS, E, D4–D7 signals directly to Arduino pins in this build. They are handled by the I²C backpack. A4 and A5 are reserved for I²C communication while the display is connected.

The firmware calls `randomSeed(micros())` when a new game starts, after the player has waited and pressed a start button. This is a lightweight timing-derived seed, not true or cryptographic randomness; repeated startup timing can produce similar sequences.
## Software requirements

- Arduino Uno / ATmega328P-compatible board.
- Arduino IDE.
- Install **hd44780 by Bill Perry** through **Sketch → Include Library → Manage Libraries** (Library Manager). The firmware needs the `hd44780` and `hd44780_I2Cexp` headers.
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

Do not select a board merely because its name looks similar. The board selection controls the compile target and upload protocol. If using a classic Nano or a Nano clone, check its processor/bootloader option under **Tools → Processor** where that menu exists. For an older Nano that uses the old bootloader, select **ATmega328P (Old Bootloader)**. This is a Nano processor/bootloader selection, not an Arduino IDE version setting and not an option for the Uno. See the [Arduino Nano processor guidance](https://support.arduino.cc/hc/en-us/articles/4401874304274-Select-the-right-processor-for-Arduino-Nano).

### Arduino IDE version note

This project does not require an old IDE to work. Arduino IDE 2.x is the actively developed and recommended environment. If you specifically need the classic interface, **Arduino IDE 1.8.19 is the final legacy IDE 1.x release** and remains available from the [official Arduino software page](https://www.arduino.cc/en/software). It can compile and upload compatible AVR sketches, but it is not required for bootloading and it does not repair a missing or damaged bootloader.

For this project on a classic **Arduino Uno R3**, select **Arduino Uno** in either IDE version. The Uno does not use the Nano's “Old Bootloader” processor menu option. If your hardware is actually a classic Arduino Nano or Nano clone, see the Nano-specific note in the bootloader section below.

## Bootloader: what it does and when to burn it

The bootloader is a small program stored on the microcontroller that allows a compatible sketch to be uploaded over the serial/USB interface. A normal Arduino Uno that already has a working bootloader **does not need its bootloader burned before every sketch upload**. Uploading a sketch and burning/replacing a bootloader are different operations. Arduino boards normally ship with a bootloader already programmed. Follow the [official Arduino bootloader recovery guide](https://support.arduino.cc/hc/en-us/articles/4841602539164-Burn-the-bootloader-on-UNO-Mega-and-classic-Nano-using-another-Arduino) only when recovery or first-time programming is actually needed.

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
| Compile error for `hd44780.h` or `hd44780ioClass/hd44780_I2Cexp.h` | Install **hd44780 by Bill Perry** through Arduino IDE Library Manager, then restart the IDE if necessary. |
| LCD backlight works but no text | Check SDA/SCL are not swapped, confirm 5 V and GND, adjust the backpack contrast trimmer, and run the library's `I2CexpDiag` example. |
| LCD initialization fails | Run the library's **I2CexpDiag** example (under the hd44780 I²C-expander examples) to test the I²C bus and backpack. |
| Random LED behavior seems repetitive | Arduino `random()` is pseudorandom. The firmware seeds it from elapsed startup timing; it is not a true random source. |

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
