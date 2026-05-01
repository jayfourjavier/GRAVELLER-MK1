#ifndef IBT2_H
#define IBT2_H

#include <Arduino.h>

// ===================== DEBUG SWITCH =====================
#ifdef IBT2_SERIAL
#define IBT2_LOG(x) Serial.println(String("[IBT2]\t") + x)
#define IBT2_LOGF(...) Serial.printf("[IBT2]\t" __VA_ARGS__)
#else
#define IBT2_LOG(x)
#define IBT2_LOGF(...)
#endif

class IBT2
{
private:
    int rPin;
    int lPin;
    const char *name;

    int rChannel;
    int lChannel;

    int freq;
    int resolution;
    int maxDuty;

    bool stateForward;
    bool stateBackward;

    static int nextChannel;

    int percentToDuty(int percent)
    {
        percent = constrain(percent, 0, 100);
        return map(percent, 0, 100, 0, maxDuty);
    }

    void writeForward(int duty)
    {
        ledcWrite(lChannel, 0);
        ledcWrite(rChannel, duty);

        stateForward = true;
        stateBackward = false;

        IBT2_LOGF("%s: FORWARD\n", name);
    }

    void writeBackward(int duty)
    {
        ledcWrite(rChannel, 0);
        ledcWrite(lChannel, duty);

        stateForward = false;
        stateBackward = true;

        IBT2_LOGF("%s: BACKWARD\n", name);
    }

public:
    IBT2(int rpwmPin, int lpwmPin, const char *motorName, int pwmFreq = 1000, int pwmRes = 8)
    {
        rPin = rpwmPin;
        lPin = lpwmPin;
        name = motorName;

        freq = pwmFreq;
        resolution = pwmRes;
        maxDuty = (1 << pwmRes) - 1;

        rChannel = nextChannel++;
        lChannel = nextChannel++;

        if (nextChannel > 14)
            nextChannel = 0;

        stateForward = false;
        stateBackward = false;
    }

    void begin()
    {
        ledcSetup(rChannel, freq, resolution);
        ledcSetup(lChannel, freq, resolution);

        ledcAttachPin(rPin, rChannel);
        ledcAttachPin(lPin, lChannel);

        stop();

        IBT2_LOGF("INITIALIZED: %s (RCH:%d LCH:%d)\n", name, rChannel, lChannel);
    }

    // ===================== FORWARD =====================

    void forward()
    {
        writeForward(maxDuty);
    }

    void forward(int speedPercent)
    {
        writeForward(percentToDuty(speedPercent));
    }

    // ===================== BACKWARD =====================

    void backward()
    {
        writeBackward(maxDuty);
    }

    void backward(int speedPercent)
    {
        writeBackward(percentToDuty(speedPercent));
    }

    // ===================== STOP =====================

    void stop()
    {
        ledcWrite(rChannel, 0);
        ledcWrite(lChannel, 0);

        stateForward = false;
        stateBackward = false;

        IBT2_LOGF("%s MOTOR STOPPED.\n", name);
    }

    // ===================== STATE =====================

    bool isForward() { return stateForward; }
    bool isBackward() { return stateBackward; }
};

// static channel allocator
int IBT2::nextChannel = 0;

#endif