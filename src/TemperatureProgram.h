//
// Created by elennaro on 2019-03-14.
//

#ifndef THERMOSTAT_PROJECT_TEMPERATUREPROGRAM_H
#define THERMOSTAT_PROJECT_TEMPERATUREPROGRAM_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include <RtcDS3231.h>

const float MINIMAL_DESIRED_TEMPERATURE = 10.0;
const float MAXIMAL_DESIRED_TEMPERATURE = 40.0;

enum Mode : bool {
		MANUAL = false,
		AUTO = true
};

class TemperatureProgram {

private:
		//Injected Dependencies
		RtcDS3231<TwoWire> *Rtc;

		//Properties
		bool desiredTemperatureWasChanged = false;
		RtcDateTime desiredTemperatureChangeTime;
		Mode mode = MANUAL;

protected:
		float desiredTemperature = 0;

		virtual void update();

public:
		void setup(RtcDS3231<TwoWire> *RtcPointer, float initialDesiredTemperature);

//		void program(uint8_t dayOfWeek, uint8_t hour, uint8_t quarterHour, float *desiredTemperature);

		void switchMode();

		Mode getMode() const;

		float incrementDesiredTemperature(float inc);

		float getDesiredTemperature() const;
};


#endif //THERMOSTAT_PROJECT_TEMPERATUREPROGRAM_H
