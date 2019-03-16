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
				shouldUpdateDesiredTemperature = true;
				shouldUpdateState = true;
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat->switchMode();
				shouldUpdateThermostatMode = true;
				resetWhatToChangeOnMainScreenTimeout.restart();
		}
}

void MainScreen::down() {
		if (whatToChangeOnMainScreen == DESIRED_TEMPERATURE) {
				thermostat->incrementDesiredTemperature(-0.5);
				shouldUpdateDesiredTemperature = true;
				shouldUpdateState = true;
		} else if (whatToChangeOnMainScreen == THERMOSTAT_MODE) {
				thermostat->switchMode();
				shouldUpdateThermostatMode = true;
				resetWhatToChangeOnMainScreenTimeout.restart();
		}
		updateDesiredTemperature();
}

void MainScreen::click() {
		whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
		shouldUpdateThermostatMode = true;
		resetWhatToChangeOnMainScreenTimeout.restart();
}

void MainScreen::longPress() {
		whatToChangeOnMainScreen = !whatToChangeOnMainScreen;
		shouldUpdateThermostatMode = true;
		resetWhatToChangeOnMainScreenTimeout.restart();
}

void MainScreen::updateDate() {
		display->setCursor(1, 0);
		displayTwoDigits(rtc->GetDateTime().Day());
		display->print(F("-"));
		displayTwoDigits(rtc->GetDateTime().Month());
		display->print(F("-"));
		display->print(rtc->GetDateTime().Year());
		shouldUpdateDate = false;
}

void MainScreen::updateTime() {
		display->setCursor(12, 0);
		displayTwoDigits(rtc->GetDateTime().Hour());
		display->print(F(":"));
		displayTwoDigits(rtc->GetDateTime().Minute());
		display->print(F(":"));
		displayTwoDigits(rtc->GetDateTime().Second());
		shouldUpdateTime = false;
}

void MainScreen::updateSensorsData() {
		display->setCursor(0, 1);
		display->print(F("T"));
		display->print(thermostat->getAverageTemperature(), 1);
		display->setCursor(12, 1);
		display->print(F(" H"));
		display->print(thermostat->getHumidity(), 1);
		shouldUpdateSensorsData = false;
		shouldUpdateState = true;
}

void MainScreen::updateDesiredTemperature() {
		display->setCursor(6, 1);
		display->print(F(" D"));
		display->print(thermostat->getDesiredTemperature(), 1);
		shouldUpdateDesiredTemperature = false;
}

void MainScreen::updateState() {
		display->setCursor(13, 3);
		display->print(thermostat->isHeatingNeeded() ? F("heating") : F("        "));
		shouldUpdateState = false;
}

void MainScreen::updateThermostatMode() {
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
				shouldUpdateThermostatMode = true;
				display->setCursor(0, 3);
		} else {
				shouldUpdateThermostatMode = false;
		}
}


void MainScreen::updateDisplay() {

		if (shouldUpdateDate) updateDate();
		if (shouldUpdateTime) updateTime();
		if (shouldUpdateSensorsData) updateSensorsData();
		if (shouldUpdateDesiredTemperature) updateDesiredTemperature();
		if (shouldUpdateState) updateState();
		if (shouldUpdateThermostatMode) updateThermostatMode();

		updateSensorsData();

		updateDesiredTemperature();

		updateState();

		updateThermostatMode();

}

void MainScreen::loop() {
		if (updateTimeTimeout.hasPassed(1000, true)) {
				if (rtc->GetDateTime().Day() != oldDay) {
						shouldUpdateDate = true;
						oldDay = rtc->GetDateTime().Day();
				}
				shouldUpdateTime;
		}
		if (resetWhatToChangeOnMainScreenTimeout.hasPassed(5000, true)) {
				whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
				shouldUpdateThermostatMode = false;
		}
		Screen::loop();
}

ScreenName MainScreen::getCurrentScreenName() {
		return ScreenName::MAIN_SCREEN;
}

