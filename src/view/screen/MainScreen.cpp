#include "MainScreen.h"

MainScreen::MainScreen(
				LiquidCrystal_I2C *displayPointer,
				RtcDS3231<TwoWire> *rtcPointer,
				Thermostat *thermostatPointer)
				: Screen(displayPointer, rtcPointer, thermostatPointer) {
		whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
}

void MainScreen::up() {
		if (whatToChangeOnMainScreen == DESIRED_TEMPERATURE) {
				thermostat->incrementDesiredTemperature(+0.5);
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat->switchMode();
				resetWhatToChangeOnMainScreen.restart();
		}
		stateChanged = true;
}

void MainScreen::down() {
		if (whatToChangeOnMainScreen == DESIRED_TEMPERATURE) {
				thermostat->incrementDesiredTemperature(-0.5);
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat->switchMode();
				resetWhatToChangeOnMainScreen.restart();
		}
		stateChanged = true;
}

void MainScreen::click() {
		whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
		resetWhatToChangeOnMainScreen.restart();
		stateChanged = true;
}

void MainScreen::longPress() {
		whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
		resetWhatToChangeOnMainScreen.restart();
		stateChanged = true;
}

void MainScreen::updateTime() {
		display->setCursor(1, 0);
		displayTwoDigits(rtc->GetDateTime().Day());
		display->print(F("-"));
		displayTwoDigits(rtc->GetDateTime().Month());
		display->print(F("-"));
		display->print(rtc->GetDateTime().Year());
		display->print(F(" "));
		displayTwoDigits(rtc->GetDateTime().Hour());
		display->print(F(":"));
		displayTwoDigits(rtc->GetDateTime().Minute());
		display->print(F(":"));
		displayTwoDigits(rtc->GetDateTime().Second());
}

void MainScreen::updateSensorsData() {
		display->setCursor(0, 1);
		display->print(F("T"));
		display->print(thermostat->getAverageTemperature(), 1);
		display->setCursor(12, 1);
		display->print(F(" H"));
		display->print(thermostat->getHumidity(), 1);
}

void MainScreen::updateDesiredTemperature() {
		display->setCursor(6, 1);
		display->print(F(" D"));
		display->print(thermostat->getDesiredTemperature(), 1);
}

void MainScreen::updateState() {
		display->setCursor(13, 3);
		display->print(thermostat->isHeatingNeeded() ? F("heating") : F("        "));
}

void MainScreen::updateMode() {
		display->setCursor(0, 3);
		if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				display->blink_on();
		} else {
				display->blink_off();
		}

		display->print(
						thermostat->getMode()
						? (whatToChangeOnMainScreen == THERMOSTAT_MODE ? F("AUTO  ") : F("auto  "))
						: (whatToChangeOnMainScreen == THERMOSTAT_MODE ? F("MANUAL") : F("manual"))
		);

		if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				display->setCursor(0, 3);
		}
}


void MainScreen::updateDisplay() {

		updateTime();

		updateSensorsData();

		updateDesiredTemperature();

		updateState();

		updateMode();

}

void MainScreen::loop() {
		Screen::loop();
		if (resetWhatToChangeOnMainScreen.hasPassed(5000, true)) {
				whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
		}

		if (updateDisplayTimer.hasPassed(500, true) || stateChanged) {
				updateDisplay();
				stateChanged = false;
				updateDisplayTimer.restart();
		}
}

ScreenName MainScreen::getCurrentScreenName() {
		return ScreenName::MAIN_SCREEN;
}

