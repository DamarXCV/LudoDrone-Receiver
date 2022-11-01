#include "DataDefines.h"
#include "SerialCommunication.h"
#include "Webserver.h"
#include "WiFi.h"

#include <HardwareSerial.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

extern "C" {
void app_main();
}

constexpr gpio_num_t LED_PIN = GPIO_NUM_13;
constexpr const char* WIFI_SSID = "TeensyDrone";
constexpr const char* WIFI_PASS = "Teensy123";
constexpr uint8_t WIFI_CHANNEL = 4;
constexpr uint8_t WIFI_MAX_STA_CONN = 2;
constexpr uint32_t SERIAL_BAUD = 57600;
constexpr int8_t SERIAL_RX_PIN = 18;
constexpr int8_t SERIAL_TX_PIN = 17;
constexpr uint16_t LOOP_HZ = 30;
constexpr const char* LOG_TAG = "main";

struct TelemetryData telemetryData;
struct ControlAndConfigData controlAndConfigData;
int64_t lastControlInputTime;

/**
 * @brief Main function
 *
 */
void app_main()
{
    esp_err_t err;
    ///////////////////////////////////////////////////
    // Initialize NVS
    ///////////////////////////////////////////////////
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ///////////////////////////////////////////////////
    // Start WiFi, Webserver and Serial
    ///////////////////////////////////////////////////
    startWiFi(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL, WIFI_MAX_STA_CONN);

    httpd_handle_t server = startWebserver();

    startSerial(&Serial1, SERIAL_BAUD, SERIAL_RX_PIN, SERIAL_TX_PIN);

    ///////////////////////////////////////////////////
    // Setup pins
    ///////////////////////////////////////////////////
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    ///////////////////////////////////////////////////
    // Main loop
    ///////////////////////////////////////////////////
    while (true) {
        uint64_t loopCycleStart = millis();

        gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));

        controlAndConfigData.Time = esp_timer_get_time();
        controlAndConfigData.LastControlInputTimeDelta = controlAndConfigData.Time - lastControlInputTime;
        sendSerial();

        receiveSerial();

        vTaskDelay((1000 / LOOP_HZ) - ((millis() - loopCycleStart) / LOOP_HZ));
    }

    ///////////////////////////////////////////////////
    // Stop Webserver and WiFi
    ///////////////////////////////////////////////////
    stopWebserver(server);

    stopWiFi();
}