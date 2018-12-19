/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

static const char *TAG="APP";

/* An HTTP GET handler */
esp_err_t get_users_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;



    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t hello = {
    .uri       = "/users",
    .method    = HTTP_GET,
    .handler   = get_users_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    // .user_ctx  = "Hello World!"
};

// /* An HTTP POST handler */
// esp_err_t echo_post_handler(httpd_req_t *req)
// {
//     char buf[100];
//     int ret, remaining = req->content_len;
//
//     while (remaining > 0) {
//         /* Read the data for the request */
//         if ((ret = httpd_req_recv(req, buf,
//                         MIN(remaining, sizeof(buf)))) <= 0) {
//             if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//                 /* Retry receiving if timeout occurred */
//                 continue;
//             }
//             return ESP_FAIL;
//         }
//
//         /* Send back the same data */
//         httpd_resp_send_chunk(req, buf, ret);
//         remaining -= ret;
//
//         /* Log data received */
//         ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
//         ESP_LOGI(TAG, "%.*s", ret, buf);
//         ESP_LOGI(TAG, "====================================");
//     }
//
//     // End response
//     httpd_resp_send_chunk(req, NULL, 0);
//     return ESP_OK;
// }
//
// httpd_uri_t echo = {
//     .uri       = "/echo",
//     .method    = HTTP_POST,
//     .handler   = echo_post_handler,
//     .user_ctx  = NULL
// };
//
// /* An HTTP PUT handler. This demonstrates realtime
//  * registration and deregistration of URI handlers
//  */
// esp_err_t ctrl_put_handler(httpd_req_t *req)
// {
//     char buf;
//     int ret;
//
//     if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
//         if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//             httpd_resp_send_408(req);
//         }
//         return ESP_FAIL;
//     }
//
//     if (buf == '0') {
//         /* Handler can be unregistered using the uri string */
//         ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
//         httpd_unregister_uri(req->handle, "/hello");
//         httpd_unregister_uri(req->handle, "/echo");
//     }
//     else {
//         ESP_LOGI(TAG, "Registering /hello and /echo URIs");
//         httpd_register_uri_handler(req->handle, &hello);
//         httpd_register_uri_handler(req->handle, &echo);
//     }
//
//     /* Respond with empty body */
//     httpd_resp_send(req, NULL, 0);
//     return ESP_OK;
// }
//
// httpd_uri_t ctrl = {
//     .uri       = "/ctrl",
//     .method    = HTTP_PUT,
//     .handler   = ctrl_put_handler,
//     .user_ctx  = NULL
// };
//
// httpd_handle_t start_webserver(void)
// {
//     httpd_handle_t server = NULL;
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//
//     // Start the httpd server
//     ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
//     if (httpd_start(&server, &config) == ESP_OK) {
//         // Set URI handlers
//         ESP_LOGI(TAG, "Registering URI handlers");
//         httpd_register_uri_handler(server, &hello);
//         httpd_register_uri_handler(server, &echo);
//         httpd_register_uri_handler(server, &ctrl);
//         return server;
//     }
//
//     ESP_LOGI(TAG, "Error starting server!");
//     return NULL;
// }
//
// void stop_webserver(httpd_handle_t server)
// {
//     // Stop the httpd server
//     httpd_stop(server);
// }
//
// static esp_err_t event_handler(void *ctx, system_event_t *event)
// {
//     httpd_handle_t *server = (httpd_handle_t *) ctx;
//
//     switch(event->event_id) {
//     case SYSTEM_EVENT_STA_START:
//         ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
//         ESP_ERROR_CHECK(esp_wifi_connect());
//         break;
//     case SYSTEM_EVENT_STA_GOT_IP:
//         ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
//         ESP_LOGI(TAG, "Got IP: '%s'",
//                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
//
//         /* Start the web server */
//         if (*server == NULL) {
//             *server = start_webserver();
//         }
//         break;
//     case SYSTEM_EVENT_STA_DISCONNECTED:
//         ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
//         ESP_ERROR_CHECK(esp_wifi_connect());
//
//         /* Stop the web server */
//         if (*server) {
//             stop_webserver(*server);
//             *server = NULL;
//         }
//         break;
//     default:
//         break;
//     }
//     return ESP_OK;
// }
//
// static void initialise_wifi(void *arg)
// {
//     tcpip_adapter_init();
//     ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//     ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = EXAMPLE_WIFI_SSID,
//             .password = EXAMPLE_WIFI_PASS,
//         },
//     };
//     ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());
// }
//
// void app_main()
// {
//     static httpd_handle_t server = NULL;
//     ESP_ERROR_CHECK(nvs_flash_init());
//     initialise_wifi(&server);
// }
