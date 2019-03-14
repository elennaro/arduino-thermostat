//
// Created by elennaro on 2019-03-14.
//

#include "ViewController.h"

ViewController::ViewController(LiquidCrystal_I2C *displayPointer, RtcDS3231<TwoWire> *RtcPointer, Thermostat *thermostatPointer) {
		display = displayPointer;
		Rtc = RtcPointer;
		thermostat = thermostatPointer;

		display->init();
		display->backlight();

		display->setCursor(3, 1);
		display->print(F("Initializing! "));
		Serial.println(F("Initializing! "));
		delay(2000);
		display->clear();

		setScreen(ScreenName::MAIN_SCREEN);
}

void ViewController::setStateChanged(bool stateChanged) {
		screen->setStateChanged(stateChanged);
}

bool ViewController::isStateChanged() {
		return screen->isStateChanged();
}

void ViewController::up() {
		screen->up();
}

void ViewController::down() {
		screen->down();
}

void ViewController::click() {
		screen->click();
}

void ViewController::longPress() {
		screen->longPress();
}

void ViewController::loop() {
		screen->loop();
		if (resetScreenToMain.hasPassed(60000, true) && screen->getCurrentScreenName() != ScreenName::MAIN_SCREEN) {
				setScreen(ScreenName::MAIN_SCREEN);
				setStateChanged(true);
		}
}

void ViewController::setScreen(ScreenName name) {
		display->clear();
		if (name == ScreenName::MAIN_SCREEN)
				screen = new MainScreen(display, Rtc, thermostat);
		else if (name == ScreenName::CLOCK_SETUP_SCREEN)
				//TODO: implement
				screen = new MainScreen(display, Rtc, thermostat);
		else if (name == ScreenName::MENU_SCREEN)
				//TODO: implement
				screen = new MainScreen(display, Rtc, thermostat);
		else if (name == ScreenName::PROGRAM_THERMOSTAT_SCREEN)
				//TODO: implement
				screen = new MainScreen(display, Rtc, thermostat);
}
