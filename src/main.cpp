#include <main.h>

volatile bool stateChanged = false;

LightChrono sensorPollingTimer; // NOLINT(cert-err58-cpp)
LightChrono updateDisplayTimer; // NOLINT(cert-err58-cpp)
LightChrono readButtonsTimer; // NOLINT(cert-err58-cpp)
LightChrono updateBrightnessTimer; // NOLINT(cert-err58-cpp)
LiquidCrystal_I2C lcd(DISPLAY_ADDRESS, DISPLAY_WIDTH, DISPLAY_HEIGHT);  // set the LCD address to 0x27 for a 16 chars and 2 line display

Thermostat thermostat;
SingleRotaryEnc *rotaryEnc;

void click() {
		Serial.println("CLICK");
		stateChanged = true;
}

void up() {
		Serial.println("UP");
		thermostat.incrementDesiredTemperature(+0.5);
		stateChanged = true;
}

void down() {
		Serial.println("DOWN");
		thermostat.incrementDesiredTemperature(-0.5);
		stateChanged = true;
}

void setup() {
		Serial.begin(115200);
		pinMode(RELAY_PIN, OUTPUT);
		pinMode(BRIGHTNESS_PIN, OUTPUT);

		rotaryEnc = new SingleRotaryEnc(CLK_PIN, DATA_PIN, BUTTON_PIN, up, down, click);

		thermostat.setup(SENSOR_PIN, 20);

		lcd.init(); // initialize the lcd
		// Print a message to the LCD.
		lcd.backlight();
		lcd.setCursor(3, 1);
		lcd.print("Initializing! ");
		Serial.println("Initializing! ");
		delay(2000);
		lcd.clear();
}

void loop() {

		rotaryEnc->loop();

		if (sensorPollingTimer.hasPassed(thermostat.getMinimalSamplingInterval(), true)) {
				thermostat.update();
				stateChanged = true;
		}

		if (updateDisplayTimer.hasPassed(500, true) || stateChanged) {
				lcd.setCursor(3, 0);
				lcd.print("T");
				lcd.print(thermostat.getAverageTemperature(), 1);
				lcd.print(" D");
				lcd.print(thermostat.getDesiredTemperature(), 1);
				lcd.print(" H");
				lcd.print(thermostat.getHumidity(), 0);
				lcd.setCursor(0, 3);
				lcd.print(thermostat.getMode() ? "auto  " : "manual ");
				lcd.setCursor(13, 3);

				if (thermostat.isHeatingNeeded()) {
						lcd.print("heating");
						digitalWrite(RELAY_PIN, HIGH);
				} else {
						lcd.print("        ");
						digitalWrite(RELAY_PIN, LOW);
				}
				stateChanged = false;
		}

		if (updateBrightnessTimer.hasPassed(250, true)) {
				uint8_t brightness = map(long(thermostat.getDesiredTemperature()), 10, 40, 10, 250);
				analogWrite(BRIGHTNESS_PIN, brightness);
		}
}