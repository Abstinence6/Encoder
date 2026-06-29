# Encoder

<!-- AI-ASSISTANT-NOTES:START -->
## Мапа роботи для AI-агентів

Останнє оновлення: 2026-06-28. Цей розділ написаний для агентів кодування, яким потрібно швидко зрозуміти репозиторій.

Імена файлів, API, символів, гілок, команд і форматів залишені без перекладу навмисно.

### Призначення

ESP8266 rotary encoder and button controller exposed to Home Assistant over MQTT for brightness, color, day mode, on/off and battery telemetry.

### Тип проєкту

PlatformIO Arduino прошивка для ESP8266.

### Основні точки входу і ролі файлів

- `platformio.ini` - D1 mini target with ArduinoHA, ESPRotary, Button2
- `src/main.cpp` - MQTT entities, encoder/button callbacks, battery ADC, EEPROM state, optional sleep; ключові символи: setup, rotate, click, dblclick, onNumberCommand, loop

### Індекс джерел

- `src/main.cpp` - головна реалізація / обробники подій; символи: rotate, click, dblclick, onNumberCommand

### Потік виконання / даних

- Encoder rotation/click handlers update ArduinoHA entities and persist selected state in EEPROM.
- Home Assistant number commands flow through onNumberCommand().
- loop() keeps MQTT/OTA active and may enter sleep depending on compile-time flags.

### Збірка, запуск або перевірка

- `pio run -e d1_mini`
- `pio run -e d1_mini_ota -t upload`

### Підказки для AI-агентів

- EEPROM addresses are manually assigned; keep them documented if adding values.
- ADC battery conversion depends on resistor divider constants.
- Optional AllowSleep changes runtime behavior and should be tested on hardware.

<!-- AI-ASSISTANT-NOTES:END -->
