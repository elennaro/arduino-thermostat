//
// Created by elennaro on 2019-03-14.
//

#ifndef THERMOSTAT_PROJECT_VIEWCONTROLLER_H
#define THERMOSTAT_PROJECT_VIEWCONTROLLER_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include <Thermostat.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>
#include <LightChrono.h>

#include "screen/Screen.h"
#include "screen/MainScreen.h"


class ViewController {
private:

		//Dependencies
		LiquidCrystal_I2C *display;
		RtcDS3231<TwoWire> *Rtc;
		Thermostat *thermostat;

		//Fields
		Screen *screen = nullptr;
		LightChrono resetScreenToMain; // NOLINT(cert-err58-cpp)

		//METHODS:
		void setScreen(ScreenName name);

public:
		ViewController(LiquidCrystal_I2C *displayPointer, RtcDS3231<TwoWire> *Rtc, Thermostat *thermostatPointer);

//Methods - Actions
		void up();

		void down();

		void click();

		void longPress();

		void updateSensorsData();

//Methods - Arduino specific

		void loop();

};


#endif //THERMOSTAT_PROJECT_VIEWCONTROLLER_H
