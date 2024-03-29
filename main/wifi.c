// wifi.c

#include "wifi.h"
#include "esp_log.h"

#define MAX_RECONNECT_ATTEMPTS 5
static int reconnect_attempts = 0;


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	switch (event_id) {
		case WIFI_EVENT_STA_START: 
			ESP_LOGI("WIFI","start");
        	esp_wifi_connect();
			break;
		
		case WIFI_EVENT_STA_DISCONNECTED: 
            if (reconnect_attempts < MAX_RECONNECT_ATTEMPTS) {
                ESP_LOGI("WIFI", "Intento de reconexión %d de %d", reconnect_attempts + 1, MAX_RECONNECT_ATTEMPTS);
                vTaskDelay(pdMS_TO_TICKS(time_to_reconect * 1000));
                esp_wifi_connect();
                reconnect_attempts++;
            } else {
                ESP_LOGE("WIFI", "Máximo número de intentos de reconexión alcanzado. Reiniciando...");
                esp_restart();
            }
            break;

        case WIFI_EVENT_STA_CONNECTED: // Reinicia el contador si la conexión es exitosa.
            reconnect_attempts = 0;
            ESP_LOGI("WIFI","Conectado, reiniciando contador de intentos,");
            break;

		case IP_EVENT_STA_GOT_IP: 
			ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
			ESP_LOGI("WIFI", "IP(" IPSTR ")", IP2STR(&event->ip_info.ip));
			xSemaphoreGive(wifi_semaphore);
			ESP_LOGI("WIFI", "wifi_semaphore unlocked");
			break;

		default: 
			printf("event_id: %d\n", (int) event_id);
			break;
		
	}
}

void wifi_setup(void) {
	printf("------------------------------\n");
	ESP_LOGI("WIFI", "wifi_setup: starting");

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_event_handler,NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event_handler,NULL));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	ESP_LOGI("WIFI", "SSID: %s -- PASS: %s", CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

	wifi_config_t wifi_config = {
        .sta = {
          .ssid = CONFIG_WIFI_SSID,
          .password = CONFIG_WIFI_PASSWORD,
          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          .pmf_cfg = {
              .capable = true,
              .required = false
          },
        }
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI("WIFI", "wifi_setup: done");
	printf("------------------------------\n");
	xSemaphoreTake(wifi_semaphore, portMAX_DELAY);
}


