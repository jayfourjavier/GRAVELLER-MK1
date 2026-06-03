
// IR.h

#pragma once

#include <Arduino.h>

#define IR_1_PIN 18
#define IR_2_PIN 19
#define IS_EMPTY_THRESHOLD_MILLIS 60000UL

volatile unsigned long lastDropMillis = 0;
volatile bool isEmpty = true;

void IRAM_ATTR irTriggered()
{
    lastDropMillis = millis();
    isEmpty = false;
}

inline void setupIr()
{
    pinMode(IR_1_PIN, INPUT_PULLUP);
    pinMode(IR_2_PIN, INPUT_PULLUP);

    lastDropMillis = millis();

    attachInterrupt(
        digitalPinToInterrupt(IR_1_PIN),
        irTriggered,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(IR_2_PIN),
        irTriggered,
        FALLING
    );
}

inline void updateIr()
{
    if (millis() - lastDropMillis > IS_EMPTY_THRESHOLD_MILLIS)
    {
        isEmpty = true;
    }
}