#include <main.h>
#include <Wire.h>
#include <RtcDS3231.h>

enum WhatToChangeOnMainScreen : bool {
		DESIRED_TEMPERATURE = true,
		THERMOSTAT_MODE = false
};


enum CurrentScreen : uint8_t {
		MAIN_SCREEN = 0,
		MENU_SCREEN = 1,
		CLOCK_SETUP_SCREEN = 2,
		PROGRAM_THERMOSTAT_SCREEN = 3
};

volatile bool stateChanged = false;
volatile bool whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
volatile uint8_t currentScreen = MAIN_SCREEN;

LightChrono sensorPollingTimer; // NOLINT(cert-err58-cpp)
LightChrono updateDisplayTimer; // NOLINT(cert-err58-cpp)
LightChrono resetWhatToChangeOnMainScreen; // NOLINT(cert-err58-cpp)
LightChrono resetScreenToMain; // NOLINT(cert-err58-cpp)
LightChrono updateBrightnessTimer; // NOLINT(cert-err58-cpp)
LiquidCrystal_I2C lcd(DISPLAY_ADDRESS, DISPLAY_WIDTH, DISPLAY_HEIGHT);  // set the LCD address to 0x27 for a 16 chars and 2 line display

RtcDS3231<TwoWire> Rtc(Wire);
Thermostat thermostat;
SingleRotaryEnc *rotaryEnc;

void click() {
		switch (currentScreen) {
				case MAIN_SCREEN:
						whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
						resetWhatToChangeOnMainScreen.restart();
						break;
				default:
						Serial.println("CLICK");
						break;
		}
		stateChanged = true;
}

void longPress() {
		whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
		resetWhatToChangeOnMainScreen.restart();
		stateChanged = true;
}

void up() {
		Serial.println("UP");
		if (whatToChangeOnMainScreen == DESIRED_TEMPERATURE) {
				thermostat.incrementDesiredTemperature(+0.5);
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat.switchMode();
				resetWhatToChangeOnMainScreen.restart();
		}
		stateChanged = true;
}

void down() {
		Serial.println("DOWN");
		if (whatToChangeOnMainScreen == DESIRED_TEMPERATURE) {
				thermostat.incrementDesiredTemperature(-0.5);
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat.switchMode();
				resetWhatToChangeOnMainScreen.restart();
		}
		stateChanged = true;
}

void setup() {
		//Init the time
		RtcDateTime compiledDateTime = RtcDateTime(__DATE__, __TIME__);
		Rtc.Begin();
		RtcDateTime now = Rtc.GetDateTime();
		if (now < compiledDateTime) {
				Rtc.SetDateTime(compiledDateTime);
		}
		Rtc.Enable32kHzPin(false);
		Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

		Serial.begin(115200);

		pinMode(RELAY_PIN, OUTPUT);
		pinMode(BRIGHTNESS_PIN, OUTPUT);

		rotaryEnc = new SingleRotaryEnc(CLK_PIN, DATA_PIN, BUTTON_PIN, up, down, click, longPress);

		thermostat.setup(&Rtc, SENSOR_PIN, 20);

		lcd.init(); // initialize the lcd
		// Print a message to the LCD.
		lcd.backlight();
		lcd.setCursor(3, 1);
		lcd.print("Initializing! ");
		Serial.println("Initializing! ");
		delay(2000);
		lcd.clear();
		whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
}

void displayTwoDigits(uint8_t numberToDisplay) {
		if (numberToDisplay < 10) {
				lcd.print(0);
		}
		lcd.print(numberToDisplay);
}

void loop() {

		rotaryEnc->loop();

		if (sensorPollingTimer.hasPassed(thermostat.getMinimalSamplingInterval(), true)) {
				thermostat.update();
				stateChanged = true;
		}

		if (updateDisplayTimer.hasPassed(500, true) || stateChanged) {
				lcd.setCursor(1, 0);
				displayTwoDigits(Rtc.GetDateTime().Day());
				lcd.print("-");
				displayTwoDigits(Rtc.GetDateTime().Month());
				lcd.print("-");
				lcd.print(Rtc.GetDateTime().Year());
				lcd.print(" ");
				displayTwoDigits(Rtc.GetDateTime().Hour());
				lcd.print(":");
				displayTwoDigits(Rtc.GetDateTime().Minute());
				lcd.print(":");
				displayTwoDigits(Rtc.GetDateTime().Second());
				lcd.setCursor(0, 1);
				lcd.print("T");
				lcd.print(thermostat.getAverageTemperature(), 1);
				lcd.print(" D");
				lcd.print(thermostat.getDesiredTemperature(), 1);
				lcd.print(" H");
				lcd.print(thermostat.getHumidity(), 1);
				lcd.setCursor(0, 3);
				if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
						lcd.blink_on();
				} else {
						lcd.blink_off();
				}
				lcd.print(thermostat.getMode() ? "auto  " : "manual ");
				lcd.setCursor(13, 3);

				if (thermostat.isHeatingNeeded()) {
						lcd.print("heating");
						digitalWrite(RELAY_PIN, HIGH);
				} else {
						lcd.print("        ");
						digitalWrite(RELAY_PIN, LOW);
				}
				if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
						lcd.setCursor(0, 3);
				}
				stateChanged = false;
		}

		if (updateBrightnessTimer.hasPassed(250, true)) {
				uint8_t brightness = map(long(thermostat.getDesiredTemperature()), 10, 40, 10, 250);
				analogWrite(BRIGHTNESS_PIN, brightness);
		}

		if (resetWhatToChangeOnMainScreen.hasPassed(5000, true)) {
				whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
		}

		if (resetScreenToMain.hasPassed(60000, true) && currentScreen != MAIN_SCREEN) {
				currentScreen = MAIN_SCREEN;
				stateChanged = true;
		}
}