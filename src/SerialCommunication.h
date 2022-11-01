#pragma once

#include <HardwareSerial.h>

/**
 * @brief Setup for the serial communication
 *
 * @param UART Serial port
 */
void startSerial(HardwareSerial* UART, uint32_t baud, int8_t rxPin, int8_t txPin);

/**
 * @brief Send the predefined serial data object
 *
 */
void sendSerial();

/**
 * @brief Read the serial data into the predefined object
 *
 */
void receiveSerial();
