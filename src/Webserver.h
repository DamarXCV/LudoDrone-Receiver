#pragma once

#include <esp_http_server.h>

/**
 * @brief Starting the webserver
 *
 * @return httpd_handle_t Handle for stopping the webserver
 */
httpd_handle_t startWebserver(void);

/**
 * @brief Stopping the webserver
 *
 * @param server Handle for the webserver
 */
void stopWebserver(httpd_handle_t server);
