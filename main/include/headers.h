// headers.h

#ifndef HEADERS_H_
#define HEADERS_H_

// Inclusión de bibliotecas estándar y del SDK de ESP-IDF
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "esp_camera.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include <sys/param.h>

// Inclusión de bibliotecas de FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

// Definición cámara AI_THINKER
#define CAMERA_MODEL_AI_THINKER

// Prototipos de funciones iniciales
void wifi_setup(void);
void camera_init(void);
// void send_to_cloud(const uint8_t* data, size_t length);



// Declaración de semáforos externos para sincronización entre tareas
extern SemaphoreHandle_t wifi_semaphore; // Para controlar el acceso a la conectividad WiFi
extern SemaphoreHandle_t sntp_semaphore; // Para sincronizar tareas relacionadas con SNTP
extern SemaphoreHandle_t mqtt_semaphore; // Para gestionar el acceso a operaciones MQTT
extern SemaphoreHandle_t dtct_semaphore; // Para controlar el acceso a recursos relacionados con detección


#endif // HEADERS_H_

