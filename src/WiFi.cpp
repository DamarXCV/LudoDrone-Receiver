#include "WiFi.h"

#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <string.h>

constexpr const char* LOG_TAG = "Wifi";

static esp_netif_t* netifAp = NULL;
static esp_event_handler_instance_t apEventHandlerInstance = NULL;

/**
 * @brief Handles events of the AP
 *
 * @param arg
 * @param eventBase
 * @param eventId
 * @param eventData
 */
static void apEventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    if (eventId == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)eventData;
        ESP_LOGI(LOG_TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (eventId == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)eventData;
        ESP_LOGI(LOG_TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void startWiFi(const char* ssid, const char* pass, uint8_t channel, uint8_t max_conn)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    netifAp = esp_netif_create_default_wifi_ap();
    assert(netifAp);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT, ESP_EVENT_ANY_ID, &apEventHandler, NULL, &apEventHandlerInstance));

    ESP_ERROR_CHECK(esp_wifi_set_storage(wifi_storage_t::WIFI_STORAGE_RAM));

    wifi_config_t wifiConfig;
    strcpy((char*)wifiConfig.ap.ssid, ssid);
    strcpy((char*)wifiConfig.ap.password, pass);
    wifiConfig.ap.ssid_len = strlen(ssid);
    wifiConfig.ap.channel = channel;
    wifiConfig.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifiConfig.ap.ssid_hidden = 0;
    wifiConfig.ap.max_connection = max_conn;
    wifiConfig.ap.beacon_interval = 10000;
    wifiConfig.ap.pairwise_cipher = wifi_cipher_type_t::WIFI_CIPHER_TYPE_NONE;
    wifiConfig.ap.ftm_responder = false;

    if (strlen(pass) == 0) {
        wifiConfig.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode_t::WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(wifi_interface_t::WIFI_IF_AP, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_TAG, "Wifi init softap finished. SSID:%s password:%s channel:%d",
        ssid, pass, channel);
}

void stopWiFi()
{
    ESP_ERROR_CHECK(esp_wifi_stop());

    ESP_ERROR_CHECK(esp_wifi_deinit());

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, apEventHandlerInstance));

    esp_netif_destroy_default_wifi(netifAp);

    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    // ESP_ERROR_CHECK(esp_netif_deinit());
}
