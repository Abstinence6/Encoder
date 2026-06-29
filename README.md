# Encoder

<!-- AI-ASSISTANT-NOTES:START -->
## AI working map

Last updated: 2026-06-28. This section is written for coding agents that need to understand the repository quickly.

### Purpose

ESP8266 rotary encoder and button controller exposed to Home Assistant over MQTT for brightness, color, day mode, on/off and battery telemetry.

### Project type

PlatformIO Arduino firmware for ESP8266.

### Primary entrypoints and file roles

- `platformio.ini` - D1 mini target with ArduinoHA, ESPRotary, Button2
- `src/main.cpp` - MQTT entities, encoder/button callbacks, battery ADC, EEPROM state, optional sleep; key symbols: setup, rotate, click, dblclick, onNumberCommand, loop

### Source index

- `src/main.cpp` - main implementation / event handlers; symbols: rotate, click, dblclick, onNumberCommand

### Runtime / data flow

- Encoder rotation/click handlers update ArduinoHA entities and persist selected state in EEPROM.
- Home Assistant number commands flow through onNumberCommand().
- loop() keeps MQTT/OTA active and may enter sleep depending on compile-time flags.

### Build, run, or validate

- `pio run -e d1_mini`
- `pio run -e d1_mini_ota -t upload`

### AI agent guidance

- EEPROM addresses are manually assigned; keep them documented if adding values.
- ADC battery conversion depends on resistor divider constants.
- Optional AllowSleep changes runtime behavior and should be tested on hardware.

<!-- AI-ASSISTANT-NOTES:END -->
