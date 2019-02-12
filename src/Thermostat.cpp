#include <DHT.h>
#include <Wire.h>

#define DEFAULT_HYSTERESIS 0.25

const float MINIMAL_DESIRED_TEMPERATURE = 10.0;
const float MAXIMAL_DESIRED_TEMPERATURE = 40.0;

enum Mode : bool {
    MANUAL = false,
    AUTO = true
};

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

class Thermostat { // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
private:
    DHT dht = DHT();

    //Self service variables
    const uint32_t MINIMAL_SAMPLING_INTERVAL = 2000;

    //Dependencies that will be injected
    TemperatureStack temperatureStack;

    //Inner variables
    float hysteresis = DEFAULT_HYSTERESIS;
    float desiredTemperature = 0;
    float humidity = 0;
    float pressure = 0;
    float previousHighTemperature = 0;
    float previousLowTemperature = 0;
    bool heatingNeeded = false;
    bool coolingNeeded = false;
    Mode mode = MANUAL;

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

    bool isHeatingNeeded() {
        return heatingNeeded;
    }

    bool isCoolingNeeded() {
        return coolingNeeded;
    }

    Mode getMode() const {
        return mode;
    }

    void setMode(Mode modeToSet) {
        mode = modeToSet;
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
    void update() {
        humidity = dht.getHumidity();
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