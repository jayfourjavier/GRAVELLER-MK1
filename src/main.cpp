#define BLYNK_TEMPLATE_ID "TMPL6dmGmv-qU"
#define BLYNK_TEMPLATE_NAME "GRAVELLER"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define IBT2_SERIAL
#define RELAY_SERIAL

// #define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
#define USE_ESP32_DEV_MODULE

unsigned long lastMillis = 0;

#include <Arduino.h>
#include "BlynkEdgent.h"
#include "defines.h"

#include "IBT2.h"
#include "Relay.h"

IBT2 right(RIGHT_R_PWM_PIN, RIGHT_L_PWM_PIN, "RIGHT");
IBT2 left(LEFT_R_PWM_PIN, LEFT_L_PWM_PIN, "LEFT");
IBT2 dispenser(DISPENSER_R_PWM_PIN, DISPENSER_L_PWM_PIN, "DISPENSER");

Relay lamp(LAMP_RELAY_PIN, "LAMP", true);
Relay camera(CAMERA_RELAY_PIN, "CAMERA", true);

char command = '\0';

void blynkCommand(bool pressed, char cmd)
{
  if (pressed)
  {
    command = cmd;
  }
  else
  {
    command = 'S';
  }
}

BLYNK_WRITE(RIGHT_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), 'R');
}

BLYNK_WRITE(LEFT_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), 'L');
}

BLYNK_WRITE(FORWARD_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), 'F');
}

BLYNK_WRITE(BACKWARD_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), 'B');
}

BLYNK_WRITE(ACTIVE_CAMERA_VIRTUAL_PIN)
{
  int val = param.asInt();

  if (val == 1)
  {
    command = '1';
  }
  else if (val == 2)
  {
    command = '2';
  }
  else
  {
    command = 'S'; // optional fallback stop
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  delay(1000);

  // SETUP MOTOR DRIVERS
  right.begin();
  left.begin();
  dispenser.begin();

  camera.begin();
  lamp.begin();
}

void loop()
{
  BlynkEdgent.run();

  switch (command)
  {

  case '1':
    Serial.println("SWITCHED TO FRONT CAM");
    // front camera logic here
    break;

  case '2':
    Serial.println("SWITCHED TO BACK CAM");
    // back camera logic here
    break;

  case 'R':
    Serial.println("RIGHT");
    break;

  case 'L':
    Serial.println("LEFT");
    break;

  case 'F':
    Serial.println("FORWARD");
    break;

  case 'B':
    Serial.println("BACKWARD");
    break;

  case 'S':
    Serial.println("STOP");
    break;

  default:
    // Serial.println("UNKNOWN");
    break;
  }

  command = 'X';
}
