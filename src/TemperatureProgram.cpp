//
// Created by elennaro on 2019-03-14.
//

#include <Wire.h>
#include "TemperatureProgram.h"


void TemperatureProgram::setup(RtcDS3231<TwoWire> *RtcPointer, float initialDesiredTemperature) {
		desiredTemperature = initialDesiredTemperature;
		Rtc = RtcPointer;
}

void TemperatureProgram::switchMode() {
		mode = mode == AUTO ? MANUAL : AUTO;
}

Mode TemperatureProgram::getMode() const {
		return mode;
}

float TemperatureProgram::incrementDesiredTemperature(float inc) {
		desiredTemperature += inc;
		if (desiredTemperature < MINIMAL_DESIRED_TEMPERATURE) {
				desiredTemperature = MINIMAL_DESIRED_TEMPERATURE;
		} else if (desiredTemperature > MAXIMAL_DESIRED_TEMPERATURE) {
				desiredTemperature = MAXIMAL_DESIRED_TEMPERATURE;
		}
		return desiredTemperature;
}

float TemperatureProgram::getDesiredTemperature() const {
		return desiredTemperature;
}

void TemperatureProgram::update() {
		if (mode == MANUAL) return;
}
