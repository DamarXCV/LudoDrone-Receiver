#include "DataDefines.h"

#include <EasyTransfer.h>
#include <HardwareSerial.h>

extern struct TelemetryData telemetryData;
extern struct ControlAndConfigData controlAndConfigData;

constexpr const char* LOG_TAG = "SerialCommunication";

static EasyTransfer ETin, ETout;

/**
 * @brief Setup for the serial communication
 *
 * @param UART Serial port
 */
void startSerial(HardwareSerial* UART, uint32_t baud, int8_t rxPin, int8_t txPin)
{
    UART->begin(baud, SERIAL_8N1, rxPin, txPin);

    ETin.begin(details(telemetryData), UART);
    ETout.begin(details(controlAndConfigData), UART);
}

/**
 * @brief Send the predefined serial data object
 *
 */
void sendSerial()
{
    ETout.sendData();
}

/**
 * @brief Read the serial data into the predefined object
 *
 */
void receiveSerial()
{
    while (ETin.receiveData()) { };
}
