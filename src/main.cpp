#include <Arduino.h>
#include <LightChrono.h>
#include <Thermostat.cpp>

#include <LiquidCrystal_I2C.h>

#define SDA A4
#define SCL A5

#define DOWN_PIN 2
#define UP_PIN 3
#define RELAY_PIN 7
#define SENSOR_PIN A0

#define BRIGHTNESS_PIN 5

LightChrono sensorPollingTimer; // NOLINT(cert-err58-cpp)
LightChrono updateDisplayTimer; // NOLINT(cert-err58-cpp)
LightChrono readButtonsTimer; // NOLINT(cert-err58-cpp)
LightChrono updateBrightnessTimer; // NOLINT(cert-err58-cpp)

//LiquidCrystal lcd(8, 9, 10, 11, 12, 13); // NOLINT(cert-err58-cpp)

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

Thermostat thermostat;

bool readButtons() {
    if (digitalRead(DOWN_PIN) == HIGH)
        thermostat.incrementDesiredTemperature(-0.5);
    else if (digitalRead(UP_PIN) == HIGH)
        thermostat.incrementDesiredTemperature(+0.5);
    else
        return false;
    return true;
};

void setup() {
    pinMode(DOWN_PIN, INPUT);
    pinMode(UP_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BRIGHTNESS_PIN, OUTPUT);

    thermostat.setup(SENSOR_PIN, 20);

    lcd.init(); // initialize the lcd
    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(3, 1);
    lcd.print("Initializing! ");
    delay(2000);
    lcd.clear();
}

void loop() {
    bool stateChanged = false;
    if (readButtonsTimer.hasPassed(150, true)) {
        stateChanged = readButtons();
    }

    if (sensorPollingTimer.hasPassed(thermostat.getMinimalSamplingInterval(), true)) {
        thermostat.update();
        stateChanged = true;
    }

    if (updateDisplayTimer.hasPassed(500) || stateChanged) {
        lcd.setCursor(3, 0);
        lcd.print("T");
        lcd.print(thermostat.getAverageTemperature(), 1);
        lcd.print(" D");
        lcd.print(thermostat.getDesiredTemperature(), 1);
        lcd.print(" H");
        lcd.print(thermostat.getHumidity(), 0);
        lcd.setCursor(0, 3);
        lcd.print(thermostat.getMode() ? "auto  " : "manual ");
        lcd.setCursor(13, 3);

        if (thermostat.isHeatingNeeded()) {
            lcd.print("heating");
            digitalWrite(RELAY_PIN, HIGH);
        } else {
            lcd.print("        ");
            digitalWrite(RELAY_PIN, LOW);
        }
        updateDisplayTimer.restart();
    }

    if (updateBrightnessTimer.hasPassed(250, true)) {
        uint8_t brightness = map(long(thermostat.getDesiredTemperature()), 10, 40, 10, 250);
        analogWrite(BRIGHTNESS_PIN, brightness);
    }
}