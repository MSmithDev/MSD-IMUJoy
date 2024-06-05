/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 *
 * At the moment we are using the default settings, but they can be canged using a BleGamepadConfig instance as parameter for the begin function.
 *
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_16
 * (16 buttons by default. Library can be configured to use up to 128)
 *
 * Possible DPAD/HAT switch position values are:
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes sets all axes at once. There are a few:
 * (x axis, y axis, z axis, rx axis, ry axis, rz axis, slider 1, slider 2)
 *
 * Library can also be configured to support up to 5 simulation controls
 * (rudder, throttle, accelerator, brake, steering), but they are not enabled by default.
 *
 * Library can also be configured to support different function buttons
 * (start, select, menu, home, back, volume increase, volume decrease, volume mute)
 * start and select are enabled by default
 */

#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad("MDS-IMUJoy", "MSmithDev", 100);
BleGamepadConfiguration bleGamepadConfig;

const int reportInterval = 5; // ms

void setup()
{
    //Serial.begin(115200);
    //Serial.println("Starting BLE work!");
    

    bleGamepad.begin();
}


int rx = 0;
int ry = 0;
int rz = 0;

void loop()
{
    if (bleGamepad.isConnected())
    {
        
        rx += 10;
        if (rx > 32767)
        {
            rx = 0;
        }
        //Serial.print("RX: ");
        //Serial.println(rx);

        
        bleGamepad.setAxes(0, 0, 0, rx, rx, rx, 0, 0);
        
    }
    delay(reportInterval);
}