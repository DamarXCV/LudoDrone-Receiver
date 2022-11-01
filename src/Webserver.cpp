#include "DataDefines.h"
#include "SerialCommunication.h"
#include "SettingsPage.h"
#include "Webserver.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <regex>
#include <string>

extern struct TelemetryData telemetryData;
extern struct ControlAndConfigData controlAndConfigData;
extern int64_t lastControlInputTime;

constexpr const char* LOG_TAG = "Webserver";

///////////////////////////////////////////////////
// Util functions
///////////////////////////////////////////////////

/**
 * @brief Decodes a url string
 *
 * @param input Encoded url string
 * @return std::string Decoded url string
 */
static std::string urlDecode(std::string& input)
{
    std::string result;
    char character;
    int i, j;
    for (i = 0; i < input.length(); i++) {
        if (int(input[i]) == '%') {
            std::sscanf(input.substr(i + 1, 2).c_str(), "%x", &j);
            character = static_cast<char>(j);
            result += character;
            i = i + 2;
        } else {
            result += input[i];
        }
    }
    return (result);
}

///////////////////////////////////////////////////
// URI handler function
///////////////////////////////////////////////////

/**
 * @brief Returns the webpage to the client
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t httpIndexHandler(httpd_req_t* req)
{
    ESP_LOGI(LOG_TAG, "GET handler called");
    std::string htmlPrepare = settingsPage;

    int index;
    std::string placeholderToReplace;
    char value[8];

    placeholderToReplace = "{{.RollP}}";
    sprintf(value, "%u", controlAndConfigData.RollP);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.RollI}}";
    sprintf(value, "%u", controlAndConfigData.RollI);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.RollD}}";
    sprintf(value, "%u", controlAndConfigData.RollD);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.PitchP}}";
    sprintf(value, "%u", controlAndConfigData.PitchP);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.PitchI}}";
    sprintf(value, "%u", controlAndConfigData.PitchI);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.PitchD}}";
    sprintf(value, "%u", controlAndConfigData.PitchD);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.YawP}}";
    sprintf(value, "%u", controlAndConfigData.YawP);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.YawI}}";
    sprintf(value, "%u", controlAndConfigData.YawI);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.YawD}}";
    sprintf(value, "%u", controlAndConfigData.YawD);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.HeightP}}";
    sprintf(value, "%u", controlAndConfigData.HeightP);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.HeightI}}";
    sprintf(value, "%u", controlAndConfigData.HeightI);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.HeightD}}";
    sprintf(value, "%u", controlAndConfigData.HeightD);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.RollTrim}}";
    sprintf(value, "%u", controlAndConfigData.RollTrim);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.PitchTrim}}";
    sprintf(value, "%u", controlAndConfigData.PitchTrim);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    placeholderToReplace = "{{.YawTrim}}";
    sprintf(value, "%u", controlAndConfigData.YawTrim);
    index = htmlPrepare.find(placeholderToReplace);
    htmlPrepare.replace(index, placeholderToReplace.length(), value);

    ESP_ERROR_CHECK(httpd_resp_set_status(req, HTTPD_200));
    ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html;charset=utf-8"));

    ESP_LOGI(LOG_TAG, "Send GET response");
    esp_err_t err = httpd_resp_send(req, htmlPrepare.c_str(), HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(LOG_TAG, "Send GET responsed done: %s", esp_err_to_name(err));
    return err;
}

/**
 * @brief Reveived start signal from client
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t httpStartHandler(httpd_req_t* req)
{
    ESP_LOGI(LOG_TAG, "GET handler called");

    controlAndConfigData.Status = StatusTypes::FLYING;

    ESP_LOGI(LOG_TAG, "Send GET response");
    ESP_ERROR_CHECK(httpd_resp_set_status(req, HTTPD_200));
    ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html"));
    esp_err_t err = httpd_resp_send(req, "", 0);
    return err;
}

/**
 * @brief Reveived stop signal from client
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t httpStopHandler(httpd_req_t* req)
{
    ESP_LOGI(LOG_TAG, "GET handler called");

    controlAndConfigData.Status = StatusTypes::STOP;

    ESP_LOGI(LOG_TAG, "Send GET response");
    ESP_ERROR_CHECK(httpd_resp_set_status(req, HTTPD_200));
    ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html"));
    esp_err_t err = httpd_resp_send(req, "", 0);
    return err;
}

/**
 * @brief Reveived controll input from client
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t httpCtrlHandler(httpd_req_t* req)
{
    char content[req->content_len + 1];

    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0) { // 0 return value indicates connection closed
        // Check if timeout occurred
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }
    content[req->content_len] = '\0';
    std::string contentString = content;
    contentString = urlDecode(contentString);

    uint8_t hIndex = contentString.find("h=");
    uint8_t yIndex = contentString.find("&y=");
    uint8_t pIndex = contentString.find("&p=");
    uint8_t rIndex = contentString.find("&r=");

    controlAndConfigData.Time = lastControlInputTime = esp_timer_get_time();
    controlAndConfigData.LastControlInputTimeDelta = 0;
    sendSerial();

    controlAndConfigData.Height = std::stoi(contentString.substr(hIndex + 2, yIndex - hIndex - 2));
    controlAndConfigData.Yaw = std::stoi(contentString.substr(yIndex + 3, pIndex - yIndex - 3));
    controlAndConfigData.Pitch = std::stoi(contentString.substr(pIndex + 3, rIndex - pIndex - 3));
    controlAndConfigData.Roll = std::stoi(contentString.substr(rIndex + 3, contentString.length() - rIndex));

    ESP_ERROR_CHECK(httpd_resp_set_status(req, HTTPD_200));
    ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html"));
    esp_err_t err = httpd_resp_send(req, "", 0);

    return err;
}

/**
 * @brief Reveived config input from client
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t httpPIDAndTrimHandler(httpd_req_t* req)
{
    ESP_LOGI(LOG_TAG, "POST handler called");
    char content[req->content_len + 1];

    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0) { // 0 return value indicates connection closed
        // Check if timeout occurred
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }
    content[req->content_len] = '\0';
    std::string contentString = content;
    contentString = urlDecode(contentString);

    ESP_LOGI(LOG_TAG, "Body: %s", content);

    uint8_t spacerIndex = contentString.find("=");
    std::string parameter = contentString.substr(0, spacerIndex);
    std::string value = contentString.substr(spacerIndex + 1, contentString.length() - spacerIndex);

    ESP_LOGI(LOG_TAG, "parameter: '%s'", parameter.c_str());
    ESP_LOGI(LOG_TAG, "value: '%s'", value.c_str());

    if (!std::strcmp(parameter.c_str(), "roll_p")) {
        controlAndConfigData.RollP = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "roll_i")) {
        controlAndConfigData.RollI = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "roll_d")) {
        controlAndConfigData.RollD = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "pitch_p")) {
        controlAndConfigData.PitchP = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "pitch_i")) {
        controlAndConfigData.PitchI = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "pitch_d")) {
        controlAndConfigData.PitchD = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "yaw_p")) {
        controlAndConfigData.YawP = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "yaw_i")) {
        controlAndConfigData.YawI = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "yaw_d")) {
        controlAndConfigData.YawD = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "height_p")) {
        controlAndConfigData.HeightP = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "height_i")) {
        controlAndConfigData.HeightI = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "height_d")) {
        controlAndConfigData.HeightD = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "roll_trim")) {
        ESP_LOGI("TAG", "controlAndConfigData.RollTrim: %i", controlAndConfigData.RollTrim);
        controlAndConfigData.RollTrim = std::stoi(value);
        ESP_LOGI("TAG", "roll_trim: %s - %i - %i", value.c_str(), std::stoi(value), controlAndConfigData.RollTrim);

    } else if (!std::strcmp(parameter.c_str(), "pitch_trim")) {
        controlAndConfigData.PitchTrim = std::stoi(value);
    } else if (!std::strcmp(parameter.c_str(), "yaw_trim")) {
        controlAndConfigData.YawTrim = std::stoi(value);
    } else {
        ESP_LOGI(LOG_TAG, "Unknown parameter: %s", parameter.c_str());
    }

    ESP_LOGI(LOG_TAG, "Redirect client!");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

///////////////////////////////////////////////////
// URI handler structure
///////////////////////////////////////////////////

static const httpd_uri_t uri_index = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = httpIndexHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_index_redirect = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = httpIndexHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_start = {
    .uri = "/start",
    .method = HTTP_GET,
    .handler = httpStartHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_stop = {
    .uri = "/stop",
    .method = HTTP_GET,
    .handler = httpStopHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_ctrl = {
    .uri = "/ctrl",
    .method = HTTP_POST,
    .handler = httpCtrlHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_pid = {
    .uri = "/pid",
    .method = HTTP_POST,
    .handler = httpPIDAndTrimHandler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_trim = {
    .uri = "/trim",
    .method = HTTP_POST,
    .handler = httpPIDAndTrimHandler,
    .user_ctx = NULL
};

///////////////////////////////////////////////////
// Webserver functions
///////////////////////////////////////////////////

httpd_handle_t startWebserver(void)
{
    // Generate default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.task_priority = 8;
    // Empty handle to esp_http_server
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_register_uri_handler(server, &uri_index);
        httpd_register_uri_handler(server, &uri_index_redirect);
        httpd_register_uri_handler(server, &uri_start);
        httpd_register_uri_handler(server, &uri_stop);
        httpd_register_uri_handler(server, &uri_ctrl);
        httpd_register_uri_handler(server, &uri_pid);
        httpd_register_uri_handler(server, &uri_trim);
    }
    // If server failed to start, handle will be NULL
    return server;
}

void stopWebserver(httpd_handle_t server)
{
    if (server) {
        // Stop the httpd server
        httpd_stop(server);
    }
}
