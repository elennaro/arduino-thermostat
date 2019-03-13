#include <Arduino.h>

class TemperatureStack {  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
public:
		static const uint8_t SIZE_OF_TEMPERATURES_BUFFER = 5;
private:
		float temperatures[SIZE_OF_TEMPERATURES_BUFFER];
		uint8_t lastPointerPosition = 0;

public:
		void setup(float desiredTemperature) {
				for (float &temperature : temperatures) {
						temperature = 20;
				}
		}

		void addTemperature(float temperature) {
				if (isnan(temperature) || temperature < 0 || temperature > 100)
						return;
				if (lastPointerPosition++ >= SIZE_OF_TEMPERATURES_BUFFER)
						lastPointerPosition = 0;
				temperatures[lastPointerPosition] = temperature;
		}

		float getAverageTemperature() {
				float sum = 0;
				for (float &temperature : temperatures) {
						sum += temperature;
				}
				return sum / SIZE_OF_TEMPERATURES_BUFFER;
		}
};