#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

// ===================== DEBUG SWITCH =====================
#ifdef RELAY_SERIAL
#define RELAY_LOG(x) Serial.println(String("[RELAY]\t") + x)
#define RELAY_LOGF(...) Serial.printf("[RELAY]\t" __VA_ARGS__)
#else
#define RELAY_LOG(x)
#define RELAY_LOGF(...)
#endif

class Relay
{
private:
    int pin;
    const char *name;

    bool state;     // ON/OFF state
    bool activeLow; // true = LOW triggers ON

    void writeState(bool on)
    {
        state = on;

        if (activeLow)
        {
            digitalWrite(pin, on ? LOW : HIGH);
        }
        else
        {
            digitalWrite(pin, on ? HIGH : LOW);
        }

        if (on)
        {
            RELAY_LOGF("%s: ON\n", name);
        }
        else
        {
            RELAY_LOGF("%s: OFF\n", name);
        }
    }

public:
    Relay(int relayPin, const char *relayName, bool isActiveLow = true)
    {
        pin = relayPin;
        name = relayName;
        activeLow = isActiveLow;
        state = false;
    }

    void begin()
    {
        pinMode(pin, OUTPUT);

        // default OFF state
        writeState(false);

        RELAY_LOGF("INITIALIZED: %s (pin %d)\n", name, pin);
    }

    void on()
    {
        if (state)
            return;
        writeState(true);
    }

    void off()
    {
        if (!state)
            return;
        writeState(false);
    }

    void toggle()
    {
        writeState(!state);
    }

    bool getState()
    {
        return state;
    }
};

#endif