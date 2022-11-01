#pragma once

#include <inttypes.h>

/**
 * @brief Starts the WiFi as AP
 *
 */
void startWiFi(const char* ssid, const char* pass, uint8_t channel, uint8_t max_conn);

/**
 * @brief Stops the WiFi
 *
 */
void stopWiFi();
