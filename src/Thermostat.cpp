#include <DHT.h>
#include <Wire.h>

#include "TemperatureProgram.h"
#include "TemperatureStack.cpp"

#define DEFAULT_HYSTERESIS 0.25

class Thermostat : public TemperatureProgram { // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
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
		void setup(RtcDS3231<TwoWire> *Rtc, uint8_t sensorPin, float initialDesiredTemperature) {
				TemperatureProgram::setup(Rtc, initialDesiredTemperature);
				temperatureStack.setup(initialDesiredTemperature);
				dht.setup(sensorPin);
		}

		float getAverageTemperature() {
				return temperatureStack.getAverageTemperature();
		}

		float getHumidity() const {
				return humidity;
		}

		float getPressure() const {
				return pressure;
		}

		uint16_t getMinimalSamplingInterval() const {
				return MINIMAL_SAMPLING_INTERVAL;
		}

		bool isHeatingNeeded() {
				return heatingNeeded;
		}

		bool isCoolingNeeded() {
				return coolingNeeded;
		}

		/**
		 * Should be put in loop with the interval taken from ::getMinimalSamplingInterval()
		 * @see Thermostat::getMinimalSamplingInterval
		 */
		void update() override {
				TemperatureProgram::update();
				humidity = dht.getHumidity();
				desiredTemperature = getDesiredTemperature();
				temperatureStack.addTemperature(dht.getTemperature());
				float averageTemperature = getAverageTemperature();

				heatingNeeded = heatingNeeded
				                ? averageTemperature <= desiredTemperature + hysteresis
				                : previousHighTemperature <= averageTemperature
				                  ? averageTemperature <= desiredTemperature
				                  : averageTemperature <= desiredTemperature - hysteresis;


				coolingNeeded = coolingNeeded
				                ? averageTemperature >= desiredTemperature - hysteresis
				                : previousLowTemperature >= averageTemperature
				                  ? averageTemperature >= desiredTemperature
				                  : averageTemperature >= desiredTemperature + hysteresis;

				if (heatingNeeded
				    || previousHighTemperature < averageTemperature
				    || averageTemperature <= desiredTemperature - hysteresis) {
						previousHighTemperature = averageTemperature;
				}

				if (coolingNeeded
				    || previousLowTemperature > averageTemperature
				    || averageTemperature >= desiredTemperature + hysteresis) {
						previousLowTemperature = getAverageTemperature();
				}
		}
};