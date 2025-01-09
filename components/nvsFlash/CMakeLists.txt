idf_component_register(
    SRCS "nvsFlash.c" "SpiffsManger.c"
    INCLUDE_DIRS "include"
    REQUIRES  spiffs nvs_flash esp_timer esp_event nvs_flash driver json esp_http_client esp_wifi) 