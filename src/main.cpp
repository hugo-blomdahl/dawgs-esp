#include <iostream>
#include "communication.hpp"
#include "activityPlanner.hpp"
#include "tcpClient.hpp"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"

static const char *TAG = "MAIN";
static const char *wifi_ssid = "test";
static const char *wifi_password = "test2";

EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "WiFi disconnected, retrying...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void initWifi(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create netif BEFORE wifi init
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler, NULL, NULL));                                                    

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, wifi_password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;    

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));


    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Global WiFi initialized");
}

extern "C" void app_main(void) {
    initWifi(); //must be done early!!

    Comms comms;
    comms.broadcastMsg("hello world!");
    ActivityPlanner* activityPlanner = new ActivityPlanner(&comms);
    

    TcpClient* testClient = new TcpClient("10.106.78.80", 8089, wifi_event_group, WIFI_CONNECTED_BIT);
    TcpClient* loggClient = new TcpClient("10.106.78.80", 8084, wifi_event_group, WIFI_CONNECTED_BIT);
    testClient->start();
    loggClient->start();

    ESP_LOGI(TAG, "All components started");

    while(true) {
        activityPlanner->state_machine_();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
