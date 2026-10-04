# ESP8266 Speaker Controller

An ESP8266-based web controller for a speaker's physical buttons. It creates a
Wi-Fi access point when it cannot connect to saved Wi-Fi, lets you trigger
button presses from a browser, stores Wi-Fi settings in EEPROM, and reports a
battery voltage read from the analog input.

## Features

- Seven browser controls, each with short (300 ms), long (2 s), and extra-long
  (10 s) press durations.
- Connects to saved Wi-Fi, or starts an access point if a connection is not
  available within 10 seconds.
- Wi-Fi settings can be updated from the web page and are saved to EEPROM.
- Battery reading is refreshed every five seconds.

## Hardware

- ESP8266 NodeMCU-compatible development board
- Speaker/control board with seven momentary button contacts
- Battery voltage divider connected to A0 (if battery monitoring is required)
- Jumper wires; optional transistor or optocoupler interfaces if the speaker
  button circuit is not electrically compatible with the ESP8266

### Circuit structure

```text
                         ESP8266 NodeMCU
                    +--------------------------------------+
 Speaker button 1 --| D3 / GPIO0 -> start/stop button wire |
 Speaker button 2 --| D1 / GPIO5 ------|                   |
 Speaker button 3 --| D2 / GPIO4 -102--|                   |
 Speaker button 4 --| D0 / GPIO16-202--|                   |
 Speaker button 5 --| D5 / GPIO14-337--|                   |
 Speaker button 6 --| D6 / GPIO12-512--|                   |
 Speaker button 7 --| D7 / GPIO13-752--|-> control wire    |
 Battery divider ---| A0                                   |
 Speaker ground ----| GND               -> GND wire        |
                    +--------------------------------------+
```

The seven button connections are controlled by `btnPins` in the sketch. Each
GPIO is briefly configured as an output and driven LOW to press the
corresponding button, then returned to input mode to release it. This assumes
the speaker buttons are activated by pulling their signal to ground.

| Web control | Button index | ESP8266 pin | GPIO |
| --- | ---: | --- | ---: |
| On/Off & Light | 0 | D3 | GPIO0 |
| Clock | 1 | D1 | GPIO5 |
| Alarm | 2 | D2 | GPIO4 |
| Minus (-) | 3 | D0 | GPIO16 |
| Plus (+) | 4 | D5 | GPIO14 |
| Timer / BT | 5 | D6 | GPIO12 |
| Mode / Play/Pause | 6 | D7 | GPIO13 |

**Important:** The pin names in the HTML comments do not match the `btnPins`
array. The table above reflects the actual array and therefore the current
firmware behavior.

#### Button interface

For a speaker board whose button contacts are confirmed to be safe to share
with the ESP8266, connect each GPIO to the corresponding button signal and
connect ESP8266 GND to the button circuit's ground. This makes the GPIO pull
the signal LOW during a press. Do not connect the ESP8266 directly if the
speaker board uses a different voltage, has a driven output, or does not share
the same ground; use a suitable transistor or optocoupler interface instead.
Check the speaker board's button signals with a multimeter before wiring.

Avoid holding D3/GPIO0 LOW while the ESP8266 resets or starts, as this pin
affects boot mode.

#### Battery-voltage input

Connect the battery through a resistor divider to A0, with the divider's
ground connected to ESP8266 GND:

```text
Battery + ---- Rtop ----+---- A0
                        |
                      Rbottom
                        |
Battery - --------------+---- GND
```

The firmware calculates voltage as `analogRead(A0) * (3.3 / 1023.0) * 3.2`.
This assumes the complete battery-to-ADC divider/scaling matches that factor.
Verify the A0 maximum input voltage for your exact ESP8266 board and ensure the
divider output never exceeds it; bare ESP8266 ADC inputs and NodeMCU boards can
have different limits. Calibrate the multiplier against a meter if accurate
readings are needed. Battery monitoring is optional; leave A0 unconnected if
not used.

## Getting started

1. Open `speaker_controller_node_mcu.ino` in the Arduino IDE.
2. Install/select the ESP8266 Arduino core and choose the board matching your
   development board.
3. Connect the board over USB and upload the sketch. `ESP8266WiFi`,
   `ESP8266WebServer`, and `EEPROM` are provided by the ESP8266 core.
4. Open the Serial Monitor at **115200 baud** to see the network mode and IP
   address.
5. Connect to the reported address in a browser:
   - If saved Wi-Fi credentials connect, use the ESP8266's local IP address.
   - Otherwise, connect to the `ESP_Speaker_AP` access point (default password:
     `12345678`) and open `192.168.4.1`.
6. Use the controls on the page. The Wi-Fi Settings panel can save credentials
   and restart the board.

Change the default access-point password in the sketch before deployment.
Saved Wi-Fi credentials are stored in EEPROM and are not encrypted.

## HTTP endpoints

| Endpoint | Purpose |
| --- | --- |
| `/` | Serves the controller page |
| `/press?btn=<0-6>&dur=<milliseconds>` | Activates a button for the requested duration |
| `/battery` | Returns the calculated battery voltage |
| `/getwifi` | Returns the saved Wi-Fi SSID |
| `/savewifi?ssid=<ssid>&pass=<password>` | Saves credentials and restarts |

This controller uses plain HTTP and has no authentication. Use it only on a
trusted network.
