//
// Created by elennaro on 2019-03-03.
//

#ifndef THERMOSTAT_PROJECT_MAIN_H
#define THERMOSTAT_PROJECT_MAIN_H

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#endif

#include <Wire.h>
#include <RtcDS3231.h>
#include <LightChrono.h>
#include <LiquidCrystal_I2C.h>

#include <Thermostat.h>
#include <view/ViewController.h>
#include <SingleRotaryEnc.h>

#define CLK_PIN  2
#define DATA_PIN 3
#define BUTTON_PIN 4
#define EEPROM_ADDRESS 0x50

#define DISPLAY_ADDRESS 0x27
#define DISPLAY_WIDTH 20
#define DISPLAY_HEIGHT 4

#define RELAY_PIN 7
#define SENSOR_PIN A0

#define BRIGHTNESS_PIN 5

#endif //THERMOSTAT_PROJECT_MAIN_H
