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

#include <Arduino.h>
#include "BlynkEdgent.h"
#include "defines.h"

#include "IBT2.h"
#include "Relay.h"
#include "soc/soc.h"          // Disable brownout problems
#include "soc/rtc_cntl_reg.h" // Disable brownout problems

unsigned long lastMillis = 0;
int ContainerSpeed = 20;
int TravelSpeed = 20;

enum ContainerMode
{
  LOAD,
  DROP
};

ContainerMode containerMode = DROP;
ContainerMode lastContainerMode = DROP;
bool containerEnabled = false;
bool lastContainerEnabled = false;

// ========================= MOTOR DRIVERS =========================

IBT2 right(RIGHT_R_PWM_PIN, RIGHT_L_PWM_PIN, "RIGHT");
IBT2 left(LEFT_R_PWM_PIN, LEFT_L_PWM_PIN, "LEFT");
IBT2 container(CONTAINER_R_PWM_PIN, CONTAINER_L_PWM_PIN, "CONTAINER");

// ========================= RELAYS =========================

Relay lamp(LAMP_RELAY_PIN, "LAMP", true);
Relay camera(CAMERA_RELAY_PIN, "CAMERA", true);

// ========================= COMMAND ENUM =========================

enum Command
{
  NONE,

  // MOVEMENT
  MOVE_RIGHT,
  MOVE_LEFT,
  MOVE_FORWARD,
  MOVE_BACKWARD,
  STOP,

  // CAMERA
  FRONT_CAM,
  BACK_CAM,

  // LAMP
  LAMP_ON,
  LAMP_OFF,
  CAMERA_ON,
  CAMERA_OFF,

  // CONTAINER
  MODE_LOAD,
  MODE_DROP,

  CONTAINER_ON,
  CONTAINER_OFF
};

Command command = NONE;

// ========================= HELPERS =========================

void blynkCommand(bool pressed, Command cmd)
{
  if (pressed)
  {
    command = cmd;
  }
  else
  {
    command = STOP;
  }
}

// ========================= BLYNK =========================

BLYNK_WRITE(RIGHT_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), MOVE_RIGHT);
}

BLYNK_WRITE(LEFT_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), MOVE_LEFT);
}

BLYNK_WRITE(FORWARD_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), MOVE_FORWARD);
}

BLYNK_WRITE(BACKWARD_VIRTUAL_PIN)
{
  blynkCommand(param.asInt(), MOVE_BACKWARD);
}

BLYNK_WRITE(ACTIVE_CAMERA_VIRTUAL_PIN)
{
  int val = param.asInt();

  if (val == 1)
  {
    command = FRONT_CAM;
  }
  else if (val == 2)
  {
    command = BACK_CAM;
  }
  else
  {
    command = STOP;
  }
}

BLYNK_WRITE(LAMP_VIRTUAL_PIN)
{
  int val = param.asInt();

  if (val == 1)
  {
    command = LAMP_ON;
  }
  else
  {
    command = LAMP_OFF;
  }
}

BLYNK_WRITE(CAMERA_POWER_VIRTUAL_PIN)
{
  int val = param.asInt();

  if (val == 1)
  {
    command = CAMERA_ON;
    camera.on();
  }
  else
  {
    command = CAMERA_OFF;
    camera.off();
  }
}

BLYNK_WRITE(MODE_VIRTUAL_PIN)
{
  int val = param.asInt();

  container.stop();
  containerEnabled = false;
  lastContainerEnabled = false;

  Blynk.virtualWrite(DROP_VIRTUAL_PIN, 0);

  if (val == 1)
  {
    containerMode = LOAD;
    Serial.println("MODE: LOAD");
  }
  else
  {
    containerMode = DROP;
    Serial.println("MODE: DROP");
  }
}

BLYNK_WRITE(DROP_VIRTUAL_PIN)
{
  containerEnabled = param.asInt();

  Serial.println(containerEnabled ? "CONTAINER ENABLED" : "CONTAINER DISABLED");
}

BLYNK_WRITE(SPEED_VIRTUAL_PIN)
{
  int val = param.asInt();
  ContainerSpeed = val;
  Serial.printf("DISPENSER SPEED: %d%%\n", ContainerSpeed);

  container.stop();
  containerEnabled = false;
  lastContainerEnabled = false;
}



void handleNavigation(char _navCommand)
{
  switch (_navCommand)
  {
  case 'R':
    Serial.println("RIGHT");
    // right motor logic
    right.backward(TravelSpeed);
    left.forward(TravelSpeed);

    break;

  case 'L':
    Serial.println("LEFT");
    right.forward(TravelSpeed);
    left.backward(TravelSpeed);
    // left motor logic
    break;

  case 'F':
    Serial.println("FORWARD");
    right.forward(TravelSpeed);
    left.forward(TravelSpeed);
    // forward logic
    break;

  case 'B':
    Serial.println("BACKWARD");
    right.backward(TravelSpeed);
    left.backward(TravelSpeed);
    // backward logic
    break;

  case 'S':
    Serial.println("STOP");
    right.stop();
    left.stop();
    // stop motors
    break;

  default:
    Serial.println("UNKNOWN COMMAND");
    break;
  }
}

BLYNK_WRITE(TRAVEL_SPEED_VIRTUAL_PIN)
{
  int val = param.asInt();
  TravelSpeed = map(val, 0, 100, 0, 255);
  Serial.printf("SPEED: %d%% PWM: %d\n", val, TravelSpeed);

  handleNavigation('S');
}


void handleContainer()
{
  // ALWAYS STOP if disabled (but only once)
  if (!containerEnabled)
  {
    if (lastContainerEnabled != containerEnabled)
    {
      Serial.println("CONTAINER STOP");
      container.stop();
    }

    lastContainerEnabled = containerEnabled;
    return;
  }

  // MODE CHANGE ONLY
  if (containerMode != lastContainerMode || lastContainerEnabled != containerEnabled)
  {
    if (containerMode == LOAD)
    {
      Serial.printf("MODE: LOADING (SPEED: %d%%)\n", ContainerSpeed);
      container.backward(ContainerSpeed);
    }
    else if (containerMode == DROP)
    {
      Serial.printf("MODE: DROPPING (SPEED: %d%%)\n", ContainerSpeed);
      container.forward(ContainerSpeed);
    }

    lastContainerMode = containerMode;
  }

  lastContainerEnabled = containerEnabled;
}

BLYNK_CONNECTED()
{
  Blynk.virtualWrite(MODE_VIRTUAL_PIN, 0);
  Blynk.virtualWrite(DROP_VIRTUAL_PIN, 0);
  Blynk.virtualWrite(ACTIVE_CAMERA_VIRTUAL_PIN, 1);
  Blynk.virtualWrite(LAMP_VIRTUAL_PIN, 0);
  Blynk.virtualWrite(SPEED_VIRTUAL_PIN, 50);
}

// ========================= SETUP =========================

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();
  BlynkEdgent.run();

  delay(1000);

  // MOTOR DRIVERS
  right.begin();
  left.begin();
  container.begin();

  // RELAYS
  lamp.begin();
  camera.begin();
  camera.on();
}

// ========================= LOOP =========================

void loop()
{
  BlynkEdgent.run();

  switch (command)
  {
    // ================= LAMP =================

  case LAMP_ON:
    Serial.println("LAMP TURNED ON");
    lamp.on();
    break;

  case LAMP_OFF:
    Serial.println("LAMP TURNED OFF");
    lamp.off();
    break;

    // ================= CAMERA =================

  case FRONT_CAM:
    Serial.println("SWITCHED TO FRONT CAM");
    // FRONT CAMERA LOGIC HERE
    Blynk.setProperty(STREAM_VIRTUAL_PIN, "url", FRONT_CAMERA_URL); // change BACK_CAMERA_URL TO FRONT_CAMERA_URL 5/8/2026
    break;

  case BACK_CAM:
    Serial.println("SWITCHED TO BACK CAM");
    Blynk.setProperty(STREAM_VIRTUAL_PIN, "url", BACK_CAMERA_URL); // change FRONT_CAMERA_URL TO BACK_CAMERA_URL 5/8/2026
    // BACK CAMERA LOGIC HERE
    break;

    // ================= MOVEMENT =================

  case MOVE_RIGHT:
    Serial.println("RIGHT");
    handleNavigation('R');
    break;

  case MOVE_LEFT:
    Serial.println("LEFT");
    handleNavigation('L');
    break;

  case MOVE_FORWARD:
    Serial.println("FORWARD");
    handleNavigation('F');
    break;

  case MOVE_BACKWARD:
    Serial.println("BACKWARD");
    handleNavigation('B');
    break;

  case STOP:
    Serial.println("STOP");
    handleNavigation('S');

    break;

    // ================= DISPENSER =================

  case MODE_LOAD:
    Serial.println("LOADING GRAVEL");
    break;

  case MODE_DROP:
    Serial.println("DROPPING GRAVEL");
    break;

  case CONTAINER_OFF:
    Serial.println("CONTAINER OFF");
    break;

  case NONE:
    break;

  default:
    break;
  }

  handleContainer();

  command = NONE;
}