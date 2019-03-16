#include <main.h>

volatile bool stateChanged = false;

LightChrono sensorPollingTimer; // NOLINT(cert-err58-cpp)
LightChrono updateRelay; // NOLINT(cert-err58-cpp)
LiquidCrystal_I2C lcd(DISPLAY_ADDRESS, DISPLAY_WIDTH, DISPLAY_HEIGHT);  // set the LCD address to 0x27 for a 16 chars and 2 line display

RtcDS3231<TwoWire> Rtc(Wire);
SingleRotaryEnc *rotaryEnc;

Thermostat thermostat;
ViewController *viewController;

void click() {
		Serial.println(F("click"));
		viewController->click();
}

void longPress() {
		Serial.println(F("longPress"));
		viewController->longPress();
}

void up() {
		Serial.println(F("up"));
		viewController->up();
}

void down() {
		Serial.println(F("down"));
		viewController->down();
}

void setup() {
		//Init the time
		RtcDateTime compiledDateTime = RtcDateTime(__DATE__, __TIME__);
		Rtc.Begin();
		RtcDateTime now = Rtc.GetDateTime();
		if (now < compiledDateTime) {
				Rtc.SetDateTime(compiledDateTime);
		}
		Rtc.Enable32kHzPin(false);
		Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

		Serial.begin(115200);

		pinMode(RELAY_PIN, OUTPUT);
		pinMode(BRIGHTNESS_PIN, OUTPUT);

		rotaryEnc = new SingleRotaryEnc(CLK_PIN, DATA_PIN, BUTTON_PIN, up, down, click, longPress);

		thermostat.setup(&Rtc, SENSOR_PIN, 20);

		viewController = new ViewController(&lcd, &Rtc, &thermostat);
}

void loop() {
		rotaryEnc->loop();
		viewController->loop();

		if (sensorPollingTimer.hasPassed(thermostat.getMinimalSamplingInterval(), true)) {
				thermostat.update();
				viewController->updateSensorsData();
		}

		if (updateRelay.hasPassed(500, true)) {
				if (thermostat.isHeatingNeeded()) {
						digitalWrite(RELAY_PIN, HIGH);
				} else {
						digitalWrite(RELAY_PIN, LOW);
				}
		}
}