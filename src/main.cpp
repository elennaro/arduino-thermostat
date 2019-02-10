#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Thermostat.cpp>

#define DOWN_PIN 2
#define UP_PIN 3
#define RELAY_PIN 7
#define SENSOR_PIN A0

#define BRIGHTNESS_PIN 5

unsigned long sensorLastPollingTime = 0;
unsigned long updateDisplayTime = 0;
unsigned long readButtonsTime = 0;
unsigned long updateBrightness = 0;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13); // NOLINT(cert-err58-cpp)
Thermostat thermostat;

boolean readButtons() {
    if (digitalRead(DOWN_PIN) == HIGH)
        thermostat.incrementDesiredTemperature(-0.5);
    else if (digitalRead(UP_PIN) == HIGH)
        thermostat.incrementDesiredTemperature(+0.5);
    else
        return false;
    return true;
};

boolean isIntervalPassed(uint16_t interval, unsigned long &previousTime) {
    unsigned long currentTime = millis();
    if (currentTime - interval >= previousTime) {
        previousTime = currentTime;
        return true;
    } else {
        return false;
    }
};

void setup() {
    pinMode(DOWN_PIN, INPUT);
    pinMode(UP_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BRIGHTNESS_PIN, OUTPUT);

    thermostat.setup(SENSOR_PIN, 20); // data pin 2

    lcd.begin(16, 2);
    lcd.print("Initializing! ");
}

void loop() {
    bool stateChanged = false;
    if (isIntervalPassed(150, readButtonsTime)) {
        stateChanged = readButtons();
    }

    if (isIntervalPassed(thermostat.getMinimalSamplingInterval(), sensorLastPollingTime)) {
        thermostat.update();
        stateChanged = true;
    }

    if (isIntervalPassed(500, updateDisplayTime) || stateChanged) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T");
        lcd.print(thermostat.getAverageTemperature(), 1);
        lcd.print(" D");
        lcd.print(thermostat.getDesiredTemperature(), 1);
        lcd.print(" H");
        lcd.print(thermostat.getHumidity(), 0);

        if (thermostat.isHeatingNeeded()) {
            lcd.setCursor(0, 1);
            lcd.print("Heating!");
            digitalWrite(RELAY_PIN, HIGH);
        } else {
            digitalWrite(RELAY_PIN, LOW);
        }
    }

    if (isIntervalPassed(250, updateBrightness)) {
        uint8_t brightness = map(thermostat.getDesiredTemperature(), 10, 40, 10, 250);
        analogWrite(BRIGHTNESS_PIN, brightness);
    }
};