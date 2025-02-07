#include <stdio.h>
#include "esp_log.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_http_client.h"

#include "nvsFlash.h"
#include "SpiffsManager.h"
#include "WiFiConfig.h"
#include "Authorization.h"
#include "SpotifyInterface.h"
#include "ServiceManager.h"

#define TAG "App"

// LED GPIO Definitions
#define GPIO_LED_GREEN  static_cast<gpio_num_t>(15)  // GPIO15
#define GPIO_LED_RED    static_cast<gpio_num_t>(16)  // GPIO16
#define GPIO_LED_BLUE   static_cast<gpio_num_t>(17)  // GPIO17 

#define RGB_LED_ON      0   // LED is Common Anode: 0 = ON
#define RGB_LED_OFF     1   // LED is Common Anode: 1 = OFF

// GPIO Configuration
gpio_config_t io_conf;

void LEDInit(void)
{
    // Proper struct initialization in C++
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_LED_RED) | (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_BLUE);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);

    // Set LEDs OFF initially
    gpio_set_level(GPIO_LED_RED, RGB_LED_OFF);
    gpio_set_level(GPIO_LED_GREEN, RGB_LED_OFF);
    gpio_set_level(GPIO_LED_BLUE, RGB_LED_OFF);
}

// Spotify Configuration
#define SpotifyConfigAddressInSpiffs "/spiffs/SpotifyConfig.txt"
SemaphoreHandle_t IsSpotifyAuthorizedSemaphore = NULL;
SemaphoreHandle_t IsWifiConnectedSemaphore = NULL;

// OAuth & Spotify Interfaces
OAuthInterfaceHandler_t OAuthInterfaceHandler;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

SpotifyHttpInfo_t Spotify_ClientInfo = {
    .url = "https://accounts.spotify.com/api/token",
    .host = "accounts.spotify.com",
    .path = "/api/token",
    .requestURI = "/",
    .responseURI = "/callback/",
    .hostnameMDNS = "spotify",
    .requestURL = "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state",
    .clientID = "2c2dadbd46244f2cb9f71251bc004caa",
    .base64Credintials = "MmMyZGFkYmQ0NjI0NGYyY2I5ZjcxMjUxYmMwMDRjYWE6MTE3MTFiZDBiNmQ0NGIzNDhhOGRlMDdjYjJjMzgzZGM=",
    .redirectURL = "http%3A%2F%2Fdeskhub.local%2Fcallback%2f",
};

extern "C" void app_main()
{
    nvsFlashInit();
    SpiffsInit();
    LEDInit();

    ServiceManger_TaskInit();

    gpio_set_level(GPIO_LED_RED, RGB_LED_ON);
    IsWifiConnectedSemaphore = xSemaphoreCreateBinary();

    ESP_LOGI(TAG, "Initializing and waiting for Wi-Fi to connect...");
    // WiFi_InitStation("xxxxx", "xxxxx", &IsWifiConnectedSemaphore);
    gpio_set_level(GPIO_LED_RED, RGB_LED_OFF);

    if (xSemaphoreTake(IsWifiConnectedSemaphore, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        gpio_set_level(GPIO_LED_RED, RGB_LED_ON);
    }

    ESP_LOGI(TAG, "Wi-Fi connected.");
    gpio_set_level(GPIO_LED_BLUE, RGB_LED_ON);    

    IsSpotifyAuthorizedSemaphore = xSemaphoreCreateBinary();

    OAuthInterfaceHandler.IsServiceAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    OAuthInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;

    OAuthInterfaceHandler.ClientConfig.url = Spotify_ClientInfo.url;
    OAuthInterfaceHandler.ClientConfig.host = Spotify_ClientInfo.host;
    OAuthInterfaceHandler.ClientConfig.path = Spotify_ClientInfo.path;
    OAuthInterfaceHandler.ClientConfig.requestURI = Spotify_ClientInfo.requestURI;
    OAuthInterfaceHandler.ClientConfig.responseURI = Spotify_ClientInfo.responseURI;
    OAuthInterfaceHandler.ClientConfig.hostnameMDNS = Spotify_ClientInfo.hostnameMDNS;
    OAuthInterfaceHandler.ClientConfig.requestURL = Spotify_ClientInfo.requestURL; 
    OAuthInterfaceHandler.ClientConfig.clientID = Spotify_ClientInfo.clientID;
    OAuthInterfaceHandler.ClientConfig.base64Credintials = Spotify_ClientInfo.base64Credintials;
    OAuthInterfaceHandler.ClientConfig.redirectURL = Spotify_ClientInfo.redirectURL;

    if(!Oauth_TaskInit(&OAuthInterfaceHandler))
    {
        ESP_LOGE(TAG, "OAuth task initialization failed.");
    }
    
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "Access Token: %s", Oauth_GetAccessToken());
        gpio_set_level(GPIO_LED_GREEN, RGB_LED_ON);
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
