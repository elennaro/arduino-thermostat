//
// Created by elennaro on 2019-03-14.
//

#ifndef THERMOSTAT_PROJECT_THERMOSTAT_H
#define THERMOSTAT_PROJECT_THERMOSTAT_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include <DHT.h>
#include <Wire.h>

#include "TemperatureProgram.h"
#include "TemperatureStack.cpp"

#define DEFAULT_HYSTERESIS 0.25

class Thermostat : public TemperatureProgram {
private:
		DHT dht = DHT();

		//Self service variables
		const uint32_t MINIMAL_SAMPLING_INTERVAL = 2000;

		//Dependencies
		TemperatureStack temperatureStack;

		//Injected Dependencies

		//Inner variables
		float hysteresis = DEFAULT_HYSTERESIS;
		float humidity = 0;
		float pressure = 0;
		float previousHighTemperature = 0;
		float previousLowTemperature = 0;
		bool heatingNeeded = false;
		bool coolingNeeded = false;

public:
		void setup(RtcDS3231<TwoWire> *Rtc, uint8_t sensorPin, float initialDesiredTemperature);

		float getAverageTemperature();

		float getHumidity() const;

		float getPressure() const;

		uint16_t getMinimalSamplingInterval() const;

		bool isHeatingNeeded();

		bool isCoolingNeeded();

		/**
		 * Should be put in loop with the interval taken from ::getMinimalSamplingInterval()
		 * @see Thermostat::getMinimalSamplingInterval
		 */
		void update() override;
};

#endif //THERMOSTAT_PROJECT_THERMOSTAT_H
