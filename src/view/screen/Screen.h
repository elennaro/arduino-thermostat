//
// Created by elennaro on 2019-03-14.
//

#ifndef THERMOSTAT_PROJECT_SCREEN_H
#define THERMOSTAT_PROJECT_SCREEN_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>
#include <LightChrono.h>

#include "Thermostat.h"

enum ScreenName : uint8_t {
		MAIN_SCREEN = 0,
		MENU_SCREEN = 1,
		CLOCK_SETUP_SCREEN = 2,
		PROGRAM_THERMOSTAT_SCREEN = 3
};

class Screen {
protected:
		//Dependencies
		Thermostat *thermostat;
		LiquidCrystal_I2C *display;
		RtcDS3231<TwoWire> *rtc;

		//Fields
		volatile bool stateChanged = false;
		LightChrono updateDisplayTimer; // NOLINT(cert-err58-cpp)

		//Methods
		void displayTwoDigits(uint8_t numberToDisplay) {
				if (numberToDisplay < 10) {
						display->print(0);
				}
				display->print(numberToDisplay);
		}

public:
		//Constructor
		Screen(LiquidCrystal_I2C *displayPointer, RtcDS3231<TwoWire> *rtcPointer, Thermostat *thermostatPointer) {
				display = displayPointer;
				rtc = rtcPointer;
				thermostat = thermostatPointer;
		};

		//Methods - Interface
		virtual void up() = 0;

		virtual void down() = 0;

		virtual void click() = 0;

		virtual void longPress() = 0;

		virtual void updateDisplay() = 0;

		virtual ScreenName getCurrentScreenName() = 0;

		//Methods - Field Accessors
		void setStateChanged(boolean value) {
				stateChanged = value;
		}

		boolean isStateChanged() {
				return stateChanged;
		}

		//Methods - Arduino specific

		virtual void loop() {
				if (updateDisplayTimer.hasPassed(500, true) || stateChanged) {
						updateDisplay();
						stateChanged = false;
						updateDisplayTimer.restart();
				}
		}
};


#endif //THERMOSTAT_PROJECT_SCREEN_H
