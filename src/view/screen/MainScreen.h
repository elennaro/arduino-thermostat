//
// Created by elennaro on 2019-03-14.
//

#ifndef THERMOSTAT_PROJECT_MAINSCREEN_H
#define THERMOSTAT_PROJECT_MAINSCREEN_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include "Screen.h"


enum WhatToChangeOnMainScreen : bool {
		DESIRED_TEMPERATURE = true,
		THERMOSTAT_MODE = false
};

class MainScreen : public Screen {
private:
		//Parameters
		volatile bool whatToChangeOnMainScreen = DESIRED_TEMPERATURE;
		LightChrono resetWhatToChangeOnMainScreen; // NOLINT(cert-err58-cpp)
		LightChrono updateDisplayTimer; // NOLINT(cert-err58-cpp)

		//Methods

public:
		//Constructors
		MainScreen(LiquidCrystal_I2C *display, RtcDS3231<TwoWire> *Rtc, Thermostat *thermostat);

		//Methods - Field Accessors
		ScreenName getCurrentScreenName() override;

		//Methods Interface
		void up() override;

		void down() override;

		void click() override;

		void longPress() override;

		void updateTime();

		void updateSensorsData();

		void updateDesiredTemperature();

		void updateState();

		void updateMode();

		void updateDisplay() override;

		//Methods Microcontroller specific
		void loop() override;
};


#endif //THERMOSTAT_PROJECT_MAINSCREEN_H
