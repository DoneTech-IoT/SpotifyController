#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "nvsFlash.h"
#include "SpiffsManager.h"
#include "WiFiConfig.h"
#include "OauthService.hpp"
#include "ServiceBase.hpp"
#include "Singleton.hpp"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <memory>
#include "esp_http_client.h"
#include "ServiceMngr.hpp"
#include "sdkconfig.h"

#define TAG "App"

SemaphoreHandle_t IsWifiConnectedSemaphore = NULL;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
static std::shared_ptr<ServiceMngr> serviceMngr;

extern "C" void app_main(void)
{
    serviceMngr = Singleton<ServiceMngr, const char*, SharedBus::ServiceID>::
                    GetInstance(static_cast<const char*>
                        (ServiceMngr::mServiceName[SharedBus::ServiceID::SERVICE_MANAGER]),
                        SharedBus::ServiceID::SERVICE_MANAGER);   
    
    ESP_LOGI(TAG, "Initializing and waiting for Wi-Fi to connect...");
    WiFi_InitStation("M4phone", "123_123_123", &IsWifiConnectedSemaphore); 
    IsWifiConnectedSemaphore = xSemaphoreCreateBinary();

    if (xSemaphoreTake(IsWifiConnectedSemaphore, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
    }

    ESP_LOGI(TAG, "Wi-Fi connected.");

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}