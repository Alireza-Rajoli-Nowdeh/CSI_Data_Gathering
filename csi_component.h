#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <cmath>
#include <sstream>
#include <iostream>

// Renaming TAG to avoid conflicts with other components
static const char *CSI_TAG = "CSI_COMPONENT";

char *project_type;

#define CSI_RAW 1
#define CSI_AMPLITUDE 0
#define CSI_PHASE 0

#define CSI_TYPE CSI_RAW

SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    if (!data) {
        ESP_LOGE(CSI_TAG, "CSI callback received null data.");
        return;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);

    std::stringstream ss;
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
            data->mac[0], data->mac[1], data->mac[2],
            data->mac[3], data->mac[4], data->mac[5]);

    ss << "CSI_DATA,"
       << project_type << ","
       << mac << ","
       << data->rx_ctrl.rssi << ","
       << data->rx_ctrl.rate << ","
       << data->rx_ctrl.sig_mode << ","
       << data->rx_ctrl.mcs << ","
       << data->rx_ctrl.cwb << ","
       << data->rx_ctrl.smoothing << ","
       << data->rx_ctrl.not_sounding << ","
       << data->rx_ctrl.aggregation << ","
       << data->rx_ctrl.stbc << ","
       << data->rx_ctrl.fec_coding << ","
       << data->rx_ctrl.sgi << ","
       << data->rx_ctrl.noise_floor << ","
       << data->rx_ctrl.ampdu_cnt << ","
       << data->rx_ctrl.channel << ","
       << data->rx_ctrl.secondary_channel << ","
       << data->rx_ctrl.timestamp << ","
       << data->rx_ctrl.ant << ","
       << data->rx_ctrl.sig_len << ","
       << data->rx_ctrl.rx_state << ","
       << real_time_set << ","
       << get_steady_clock_timestamp() << ","
       << data->len << ",[";

    int data_len = data->len;
    int8_t *my_ptr = data->buf;

#if CSI_RAW
    for (int i = 0; i < data_len; i++) {
        ss << (int)my_ptr[i] << " ";
    }
#endif
#if CSI_AMPLITUDE
    for (int i = 0; i < data_len / 2; i++) {
        ss << (int)std::sqrt(std::pow(my_ptr[i * 2], 2) + std::pow(my_ptr[(i * 2) + 1], 2)) << " ";
    }
#endif
#if CSI_PHASE
    for (int i = 0; i < data_len / 2; i++) {
        ss << (int)std::atan2(my_ptr[i * 2], my_ptr[(i * 2) + 1]) << " ";
    }
#endif

    ss << "]\n";

    printf("%s", ss.str().c_str());
    fflush(stdout);

    xSemaphoreGive(mutex);
}

void _print_csi_csv_header() {
    printf("type,role,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,"
           "aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,"
           "secondary_channel,local_timestamp,ant,sig_len,rx_state,real_time_set,"
           "real_timestamp,len,CSI_DATA\n");
    fflush(stdout);
}

void csi_init(char *type) {
    project_type = type;

#ifdef CONFIG_SHOULD_COLLECT_CSI
    esp_err_t err;

    ESP_LOGI(CSI_TAG, "Initializing CSI...");

    // Ensure Wi-Fi is initialized and started
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(err));
        return;
    }

    // Set Wi-Fi mode to SoftAP
    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to set Wi-Fi mode to AP: %s", esp_err_to_name(err));
        return;
    }

    // Corrected Wi-Fi configuration
    wifi_config_t wifi_config = {0}; // Ensure the structure is zero-initialized
    strncpy((char *)wifi_config.ap.ssid, "myssid", sizeof(wifi_config.ap.ssid));
    strncpy((char *)wifi_config.ap.password, "mypassword", sizeof(wifi_config.ap.password));
    wifi_config.ap.ssid_len = strlen("myssid");
    wifi_config.ap.channel = 6; // Explicitly set the channel
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.beacon_interval = 100;

    err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to set Wi-Fi configuration: %s", esp_err_to_name(err));
        return;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
        return;
    }

    // Enable CSI collection
    err = esp_wifi_set_csi(true);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to enable CSI: %s", esp_err_to_name(err));
        return;
    }

    // Configure CSI
    wifi_csi_config_t configuration_csi = {
        .lltf_en = 1,
        .htltf_en = 1,
        .stbc_htltf2_en = 1,
        .ltf_merge_en = 1,
        .channel_filter_en = 0,
        .manu_scale = 0,
        .shift = 0 // Ensure all fields are initialized
    };

    err = esp_wifi_set_csi_config(&configuration_csi);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to configure CSI: %s", esp_err_to_name(err));
        return;
    }

    // Set CSI callback
    err = esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(CSI_TAG, "Failed to set CSI callback: %s", esp_err_to_name(err));
        return;
    }

    // Print CSI CSV header
    _print_csi_csv_header();

    ESP_LOGI(CSI_TAG, "CSI successfully initialized.");
#endif
}


#endif // ESP32_CSI_CSI_COMPONENT_H
