#include <Thermostat.h>

void Thermostat::setup(RtcDS3231<TwoWire> *Rtc, uint8_t sensorPin, float initialDesiredTemperature) {
		TemperatureProgram::setup(Rtc, initialDesiredTemperature);
		temperatureStack.setup(initialDesiredTemperature);
		dht.setup(sensorPin);
}

float Thermostat::getAverageTemperature() {
		return temperatureStack.getAverageTemperature();
}

float Thermostat::getHumidity() const {
		return humidity;
}

float Thermostat::getPressure() const {
		return pressure;
}

uint16_t Thermostat::getMinimalSamplingInterval() const {
		return MINIMAL_SAMPLING_INTERVAL;
}

bool Thermostat::isHeatingNeeded() {
		return heatingNeeded;
}

bool Thermostat::isCoolingNeeded() {
		return coolingNeeded;
}

/**
 * Should be put in loop with the interval taken from ::getMinimalSamplingInterval()
 * @see Thermostat::getMinimalSamplingInterval
 */
void Thermostat::update() {
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