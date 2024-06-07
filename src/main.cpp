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
#include <Adafruit_BNO08x.h>
#include <math.h>
// SPI
#define BNO08X_CS 15
#define BNO08X_INT 2
#define BNO08X_RESET 4
#define MAX_JOYSTICK_VALUE 32767

// Maps a value from one range to another
int mapToJoystickValue(float angle, float angleMin, float angleMax) {
    return (int)((angle - angleMin) * (MAX_JOYSTICK_VALUE / (angleMax - angleMin)));
}

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

BleGamepad bleGamepad("MDS-IMUJoy", "MSmithDev", 100);
BleGamepadConfiguration bleGamepadConfig;


const int reportInterval = 5; // ms
// Here is where you define the sensor outputs you want to receive
void setReports(void)
{
    Serial.println("Setting desired reports");
    if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR))
    {
        Serial.println("Could not enable game vector");
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT))
    {
        Serial.println("Failed to find BNO08x chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("BNO08x Found!");

    for (int n = 0; n < bno08x.prodIds.numEntries; n++)
    {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
    }
    setReports();
    Serial.println("Reading events");
    delay(100);
    bleGamepad.begin();
}

int rx = 0;
int ry = 0;
int rz = 0;
float q[4] = {0.0f, 0.0f, 0.0f, 0.0f};



void quaternionToEuler(float q[], float &yaw, float &pitch, float &roll) {
    // Extract the values from the quaternion
    float qw = q[0];
    float qx = q[1];
    float qy = q[2];
    float qz = q[3];

    // Yaw (Z-axis rotation)
    yaw = atan2(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz) * RAD_TO_DEG;

   
    float sinp = 2.0 * (qw * qy - qz * qx);
    if (fabs(sinp) >= 1)
        roll = copysign(M_PI / 2, sinp) * RAD_TO_DEG; // Use 90 degrees if out of range
    else
        roll = asin(sinp) * RAD_TO_DEG;

    
    pitch = atan2(2.0 * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz) * RAD_TO_DEG;
}




void loop()
{
    if (bno08x.wasReset())
    {
        Serial.print("sensor was reset ");
        setReports();
    }

    if (bleGamepad.isConnected())
    {
        if (!bno08x.getSensorEvent(&sensorValue))
        {
            return;
        }

        switch (sensorValue.sensorId)
        {

        case SH2_GAME_ROTATION_VECTOR:
            // Serial.print("Game Rotation Vector - r: ");
            // Serial.print(sensorValue.un.gameRotationVector.real);
            // Serial.print(" i: ");
            // Serial.print(sensorValue.un.gameRotationVector.i);
            // Serial.print(" j: ");
            // Serial.print(sensorValue.un.gameRotationVector.j);
            // Serial.print(" k: ");
            // Serial.println(sensorValue.un.gameRotationVector.k);
            q[0] = sensorValue.un.gameRotationVector.real;
            q[1] = sensorValue.un.gameRotationVector.i;
            q[2] = sensorValue.un.gameRotationVector.j;
            q[3] = sensorValue.un.gameRotationVector.k;
            
             // Variables to hold Euler angles
            float yaw, pitch, roll;
            
            // Convert quaternion to Euler angles
            quaternionToEuler(q, yaw, pitch, roll);

            //printf("Yaw: %f, Pitch: %f, Roll: %f\n", yaw, pitch, roll);

            // Convert Euler angles to joystick values
            
            rz = mapToJoystickValue(yaw, -180.0f, 180.0f);
            rx = mapToJoystickValue(pitch, -180.0f, 180.0f);
            ry = mapToJoystickValue(roll, -180.0f, 180.0f);


            bleGamepad.setAxes(0, 0, 0, rx, ry, rz, 0, 0);
            break;
        }

        
    }
    delay(reportInterval);
}