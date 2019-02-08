#include <DHT.h>
#include <Wire.h>

const float MINIMAL_DESIRED_TEMPERATURE = 10.0;
const float MAXIMAL_DESIRED_TEMPERATURE = 40.0;
const uint8_t SIZE_OF_TEMPERATURES_BUFFER = 5;

enum Mode : boolean {
    AUTO, MANUAL
};

class TemperatureStack {  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
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
        if (lastPointerPosition >= 10)
            lastPointerPosition = 0;
        temperatures[lastPointerPosition] = temperature;
        lastPointerPosition++;
    }

    float getAverageTemperature() {
        float sum = 0;
        for (float temperature : temperatures) {
            sum += temperature;
        }
        return sum / SIZE_OF_TEMPERATURES_BUFFER;
    }
};

typedef struct {
    uint8_t hourTill = 0;
    uint8_t minuteTill = 0;
    float desiredTemperature = 20;
} ProgramUnit;

class Thermostat { // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
private:
    DHT dht;

    //Self service variables
    const uint32_t MINIMAL_SAMPLING_INTERVAL = 2000;

    //Dependencies that will be injected
    TemperatureStack temperatureStack;

    //Inner variables
    float hysteresis = 0.2;
    float desiredTemperature = 0;
    float humidity = 0;
    float pressure = 0;
    Mode mode;
    ProgramUnit program[7][6];

public:
    void setup(uint8_t sensorPin, float initialDesiredTemperature) {
        this->desiredTemperature = initialDesiredTemperature;
        temperatureStack.setup(this->getDesiredTemperature());
        dht.setup(sensorPin);
    }

    float getAverageTemperature() {
        return temperatureStack.getAverageTemperature();
    }

    float getDesiredTemperature() const {
        return desiredTemperature;
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

    boolean isHeatingNeeded() {
        return temperatureStack.getAverageTemperature() <= desiredTemperature;
    }

    boolean isCoolingNeeded() {
        return temperatureStack.getAverageTemperature() >= desiredTemperature;
    }

    float incrementDesiredTemperature(float inc) {
        desiredTemperature += inc;
        if (desiredTemperature < MINIMAL_DESIRED_TEMPERATURE) {
            desiredTemperature = MINIMAL_DESIRED_TEMPERATURE;
        } else if (desiredTemperature > MAXIMAL_DESIRED_TEMPERATURE) {
            desiredTemperature = MAXIMAL_DESIRED_TEMPERATURE;
        }
        return desiredTemperature;
    }

    /**
     * Should be put in loop with the interval taken from ::getMinimalSamplingInterval()
     * @see Thermostat::getMinimalSamplingInterval
     */
    void updateStateFromSensors() {
        humidity = dht.getHumidity();
        temperatureStack.addTemperature(dht.getTemperature());
    }
};
