#ifndef IBT2_H
#define IBT2_H

#include <Arduino.h>

/**
 * @file IBT2.h
 * @brief Simple IBT-2 (BTS7960) motor driver wrapper using ESP32 LEDC PWM.
 */

// ===================== DEBUG SWITCH =====================
#ifdef IBT2_SERIAL
/** @brief Print debug message */
#define IBT2_LOG(x) Serial.println(String("[IBT2]\t") + x)
/** @brief Print formatted debug message */
#define IBT2_LOGF(...) Serial.printf("[IBT2]\t" __VA_ARGS__)
#else
#define IBT2_LOG(x)
#define IBT2_LOGF(...)
#endif

/**
 * @class IBT2
 * @brief Control class for IBT-2 (BTS7960) motor driver.
 *
 * Provides forward, backward, stop, and state tracking using PWM.
 */
class IBT2
{
private:
    int rPin;         /**< Right PWM pin */
    int lPin;         /**< Left PWM pin */
    const char *name; /**< Motor name (for debug) */

    int rChannel; /**< LEDC channel for right pin */
    int lChannel; /**< LEDC channel for left pin */

    int freq;       /**< PWM frequency */
    int resolution; /**< PWM resolution (bits) */
    int maxDuty;    /**< Maximum duty cycle */

    bool stateForward;  /**< True if moving forward */
    bool stateBackward; /**< True if moving backward */

    static int nextChannel; /**< Static LEDC channel allocator */

    /**
     * @brief Convert percentage (0–100) to duty cycle.
     * @param percent Speed percentage
     * @return Duty cycle value
     */
    int percentToDuty(int percent)
    {
        percent = constrain(percent, 0, 100);
        return map(percent, 0, 100, 0, maxDuty);
    }

    /**
     * @brief Apply forward motion.
     * @param duty PWM duty cycle
     */
    void writeForward(int duty)
    {
        ledcWrite(lChannel, 0);
        ledcWrite(rChannel, duty);

        stateForward = true;
        stateBackward = false;

        IBT2_LOGF("%s: FORWARD\n", name);
    }

    /**
     * @brief Apply backward motion.
     * @param duty PWM duty cycle
     */
    void writeBackward(int duty)
    {
        ledcWrite(rChannel, 0);
        ledcWrite(lChannel, duty);

        stateForward = false;
        stateBackward = true;

        IBT2_LOGF("%s: BACKWARD\n", name);
    }

public:
    /**
     * @brief Constructor for IBT2 motor driver.
     * @param rpwmPin Right PWM pin
     * @param lpwmPin Left PWM pin
     * @param motorName Name identifier for debugging
     * @param pwmFreq PWM frequency (default: 1000 Hz)
     * @param pwmRes PWM resolution in bits (default: 8)
     */
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

    /**
     * @brief Initialize PWM channels and attach pins.
     */
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

    /**
     * @brief Run motor forward at full speed.
     */
    void forward()
    {
        writeForward(maxDuty);
    }

    /**
     * @brief Run motor forward at specified speed.
     * @param speedPercent Speed (0–100%)
     */
    void forward(int speedPercent)
    {
        writeForward(percentToDuty(speedPercent));
    }

    // ===================== BACKWARD =====================

    /**
     * @brief Run motor backward at full speed.
     */
    void backward()
    {
        writeBackward(maxDuty);
    }

    /**
     * @brief Run motor backward at specified speed.
     * @param speedPercent Speed (0–100%)
     */
    void backward(int speedPercent)
    {
        writeBackward(percentToDuty(speedPercent));
    }

    // ===================== STOP =====================

    /**
     * @brief Stop the motor.
     */
    void stop()
    {
        ledcWrite(rChannel, 0);
        ledcWrite(lChannel, 0);

        stateForward = false;
        stateBackward = false;

        IBT2_LOGF("%s MOTOR STOPPED.\n", name);
    }

    // ===================== STATE =====================

    /**
     * @brief Check if motor is moving forward.
     * @return true if forward
     */
    bool isForward() { return stateForward; }

    /**
     * @brief Check if motor is moving backward.
     * @return true if backward
     */
    bool isBackward() { return stateBackward; }
};

// static channel allocator
int IBT2::nextChannel = 0;

#endif