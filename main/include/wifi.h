// wifi.h

#ifndef __WIFI_H_
#define __WIFI_H_

#include "headers.h"
#include "esp_wifi.h"

#define time_to_reconect 5

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// Define CONFIG_WIFI_SSID 
#ifndef CONFIG_WIFI_SSID
#define CONFIG_WIFI_SSID "MOVISTAR_9BFD"
#endif

// Define CONFIG_WIFI_PASSWORD 
#ifndef CONFIG_WIFI_PASSWORD
#define CONFIG_WIFI_PASSWORD "fPfNoTQfcTBwCfHGX4qX"
#endif

#endif // __WIFI_H_
