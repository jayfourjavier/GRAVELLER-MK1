#ifndef DEFINES_H
#define DEFINES_H

/**
 * @file defines.h
 * @brief Centralized pin mappings, virtual pins, and configuration constants.
 *
 * This file contains all hardware pin assignments and system constants
 * for easier maintenance and modification.
 */

// ===================== MOTOR PINS =====================

/** @brief Right motor RPWM pin */
#define RIGHT_R_PWM_PIN 13
/** @brief Right motor LPWM pin */
#define RIGHT_L_PWM_PIN 14

/** @brief Left motor RPWM pin */
#define LEFT_R_PWM_PIN 32
/** @brief Left motor LPWM pin */
#define LEFT_L_PWM_PIN 33

/** @brief Container motor RPWM pin */
#define CONTAINER_R_PWM_PIN 25
/** @brief Container motor LPWM pin */
#define CONTAINER_L_PWM_PIN 26

// ===================== RELAY PINS =====================

/** @brief Camera relay control pin */
#define CAMERA_RELAY_PIN 16
/** @brief Lamp relay control pin */
#define LAMP_RELAY_PIN 17

// ===================== SENSOR PINS =====================

/** @brief Infrared sensor 1 pin */
#define IR_1_PIN 18
/** @brief Infrared sensor 2 pin */
#define IR_2_PIN 19

// ===================== BLYNK VIRTUAL PINS =====================

/** @brief Active camera selector */
#define ACTIVE_CAMERA_VIRTUAL_PIN V0
/** @brief Forward movement control */
#define FORWARD_VIRTUAL_PIN V1
/** @brief Backward movement control */
#define BACKWARD_VIRTUAL_PIN V2
/** @brief Turn right control */
#define RIGHT_VIRTUAL_PIN V3
/** @brief Turn left control */
#define LEFT_VIRTUAL_PIN V4
/** @brief Mode selector */
#define MODE_VIRTUAL_PIN V5
/** @brief Container drop control */
#define DROP_VIRTUAL_PIN V6
/** @brief Speed control */
#define SPEED_VIRTUAL_PIN V7
/** @brief Video stream control */
#define STREAM_VIRTUAL_PIN V8
/** @brief Lamp control */
#define LAMP_VIRTUAL_PIN V9
/** @brief Camera power control */
#define CAMERA_POWER_VIRTUAL_PIN V10

// ===================== CAMERA STREAM URLs =====================

/** @brief Front camera stream URL */
#define FRONT_CAMERA_URL "http://192.168.1.106/"
/** @brief Back camera stream URL */
#define BACK_CAMERA_URL "http://192.168.1.36/"

// ===================== SYSTEM CONFIG =====================

/** @brief Speed used during loading operation (%) */
#define LOADING_SPEED 50

/** @brief Default navigation speed (%) */
#define NAV_SPEED 50

#endif // DEFINES_H