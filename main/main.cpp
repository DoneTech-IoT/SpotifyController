#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "nvsFlash.h"
#include "SpiffsManager.h"
#include "WiFiConfig.h"
// #include "Authorization.h"
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

// #define GPIO_LED_GREEN  15  // GPIO15
// #define GPIO_LED_RED    16  // GPIO16
// #define GPIO_LED_BLUE   17  // GPIO17 

// #define RGB_LED_ON      0   // LED is Common Anode: 0 = ON
// #define RGB_LED_OFF     1   // LED is Common Anode: 1 = OFF

// gpio_config_t io_conf = {
//     .intr_type = GPIO_INTR_DISABLE,
//     .mode = GPIO_MODE_OUTPUT,
//     .pin_bit_mask = (1ULL << GPIO_LED_RED) | (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_BLUE),
//     .pull_down_en = GPIO_PULLUP_DISABLE,
//     .pull_up_en = GPIO_PULLUP_DISABLE,
// };

// void LEDInit(void)
// {
//     gpio_config(&io_conf);
//     gpio_set_level(GPIO_LED_RED, RGB_LED_OFF);
//     gpio_set_level(GPIO_LED_GREEN, RGB_LED_OFF);
//     gpio_set_level(GPIO_LED_BLUE, RGB_LED_OFF);
// }

#define SpotifyConfigAddressInSpiffs "/spiffs/SpotifyConfig.txt"

// SemaphoreHandle_t IsSpotifyAuthorizedSemaphore = NULL;
SemaphoreHandle_t IsWifiConnectedSemaphore = NULL;
// OAuthInterfaceHandler_t OAuthInterfaceHandler;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
static std::shared_ptr<ServiceMngr> serviceMngr;

extern "C" void app_main(void)
{
    // nvsFlashInit();
    // SpiffsInit();
    // // LEDInit();

    // gpio_set_level(GPIO_LED_RED, RGB_LED_ON);
    ESP_LOGI(TAG, "Initializing and waiting for Wi-Fi to connect...");
    // WiFi_InitStation("M4phone", "123_123_123", &IsWifiConnectedSemaphore);
    // gpio_set_level(GPIO_LED_RED, RGB_LED_OFF);

    // IsWifiConnectedSemaphore = xSemaphoreCreateBinary();

    // if (xSemaphoreTake(IsWifiConnectedSemaphore, portMAX_DELAY) != pdTRUE)
    // {
    //     ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
    //     //gpio_set_level(GPIO_LED_RED, RGB_LED_ON);
    // }

    // ESP_LOGI(TAG, "Wi-Fi connected.");
    //gpio_set_level(GPIO_LED_BLUE, RGB_LED_ON);    
    
    // IsSpotifyAuthorizedSemaphore = xSemaphoreCreateBinary();

    // OAuthInterfaceHandler.IsServiceAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    // OAuthInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;

    // if(!Oauth_TaskInit(&OAuthInterfaceHandler))
    // {
    //     ESP_LOGE(TAG, "OAuth task initialization failed.");
    // }
    
    //servicebase 
    // serviceMngr = Singleton<OauthService, const char*, SharedBus::ServiceID>::
    //                 GetInstance(static_cast<const char*>
    //                     (mServiceName[SharedBus::ServiceID::OAUTH]),
    //                     SharedBus::ServiceID::OAUTH);     
    // Log_RamOccupy("main", "service manager");        
    serviceMngr = Singleton<ServiceMngr, const char*, SharedBus::ServiceID>::
                    GetInstance(static_cast<const char*>
                        (ServiceMngr::mServiceName[SharedBus::ServiceID::SERVICE_MANAGER]),
                        SharedBus::ServiceID::SERVICE_MANAGER);   
    WiFi_InitStation("M4phone", "123_123_123", &IsWifiConnectedSemaphore); 
    IsWifiConnectedSemaphore = xSemaphoreCreateBinary();

    if (xSemaphoreTake(IsWifiConnectedSemaphore, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        //gpio_set_level(GPIO_LED_RED, RGB_LED_ON);
    }

    ESP_LOGI(TAG, "Wi-Fi connected.");
    // if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    // {
    //    // ESP_LOGI(TAG, "Access Token: %s", Oauth_GetAccessToken());
    //    // gpio_set_level(GPIO_LED_GREEN, RGB_LED_ON);
    // }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}