// main.c

// Includes de proyecto
#include "headers.h"

// Liberías incluidas en el archivo de configuración de prueba
#include <stdio.h>
#include "esp_system.h"

#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"


// Librerías necesarias para el proyecto TFG, 
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "freertos/semphr.h"
SemaphoreHandle_t wifi_semaphore = NULL; // Buenas prácticas , inicialización explícita

// Definición de variables globales
#define WIFI_SSID "MOVISTAR_9BFD"
#define WIFI_PASS "fPfNoTQfcTBwCfHGX4qX"
#define GOOGLE_CLOUD_URL "https://storage.googleapis.com/bucket-bdominguez-pei"

// Configuración para la placa tipo AI-Thinker, definición de pines
#define CAM_PIN_PWDN    -1 // Power Down pin no es usado
#define CAM_PIN_RESET   1 // Reset pin es usado (RST en la placa)
#define CAM_PIN_XCLK    21
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      19
#define CAM_PIN_D2      18
#define CAM_PIN_D1       5
#define CAM_PIN_D0       4
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

#define CAM_PIN_LED     4 // Suponiendo que quieres usar el pin D0 como LED


// Función principal
void app_main(void) 
{   
    // Inicialización del NVS
    /*
       - Propósito: almacenamiento de datos que deben de persistir (no volátiles)
       - Uso: Inicializa el NVS y maneja dos errores específicos que pueden surgir si el NVS necesita ser reformateado 
         (ESP_ERR_NVS_NO_FREE_PAGES) o si la versión del NVS ha cambiado (ESP_ERR_NVS_NEW_VERSION_FOUND). 
         En ambos casos, borra el almacenamiento NVS y lo inicializa de nuevo.
    */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    
    
    
    // Creación de semáforo:
    wifi_semaphore = xSemaphoreCreateBinary();

    if (wifi_semaphore == NULL) {
        // Manejar el error, la creación del semáforo falló
        printf("Falló la creación del semáforo.\n");
    }


    //camera_init();                             // Inicializa la cámara
    wifi_setup();                                // Inicializa la conexión WiFi
    while(true){
        ESP_LOGI("app_main", "Dentro del bucle while...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    // int count = 10;                            // Contador para la cantidad de veces que se ejecutará el bucle
    // while (count > 0) {
    //     ESP_LOGI("app_main", "Intentando tomar una foto...");
    //     camera_fb_t* fb = esp_camera_fb_get(); // Captura una imagen con la cámara
    //     if (!fb) {                             // Verifica si la captura fue exitosa. Si fb es NULL, significa que hubo un error al capturar la imagen.
    //         ESP_LOGE("CAMERA", "Error en la captura de imagen");
    //     } else {                               // Si la captura fue exitosa, llama a la función send_to_cloud()
    //         ESP_LOGI("app_main", "Imagen tomada con éxito.");
    //         send_to_cloud(fb->buf, fb->len);   // Envía los datos de la imagen capturada a Google Cloud Storage (Bucket)
    //         esp_camera_fb_return(fb);          // Devuelve el búfer de la imagen para que pueda ser reutilizado
    //     }

    //     vTaskDelay(pdMS_TO_TICKS(10000));      // Espera durante 10 segundos antes de la próxima captura

    //     count -= 1;                            // Decrementa el contador
    // }
}





// Definición de funciones


// void camera_init() {
//     camera_config_t config;
//     config.ledc_channel = LEDC_CHANNEL_0;
//     config.ledc_timer = LEDC_TIMER_0;

//     config.pin_d0 = CAM_PIN_D0;
//     config.pin_d1 = CAM_PIN_D1;
//     config.pin_d2 = CAM_PIN_D2;
//     config.pin_d3 = CAM_PIN_D3;
//     config.pin_d4 = CAM_PIN_D4;
//     config.pin_d5 = CAM_PIN_D5;
//     config.pin_d6 = CAM_PIN_D6;
//     config.pin_d7 = CAM_PIN_D7;

//     config.pin_xclk = CAM_PIN_XCLK;
//     config.pin_pclk = CAM_PIN_PCLK;
//     config.pin_vsync = CAM_PIN_VSYNC;
//     config.pin_href = CAM_PIN_HREF;
//     config.pin_sccb_sda = CAM_PIN_SIOD;
//     config.pin_sccb_scl = CAM_PIN_SIOC;
//     config.pin_pwdn = CAM_PIN_PWDN;
//     config.pin_reset = CAM_PIN_RESET;
    
//     config.xclk_freq_hz = 20000000;
//     config.pixel_format = PIXFORMAT_JPEG;
    
//     // Led que simula la puerta abriéndose
//     //config.pin_ledc = CAM_PIN_LED;

//     config.frame_size = FRAMESIZE_UXGA;
//     config.jpeg_quality = 10;
//     config.fb_count = 2;

//     // Iniciar la cámara
//     esp_err_t err = esp_camera_init(&config);
//     if (err != ESP_OK) {
//         ESP_LOGE("camera", "Camera init failed with error 0x%x", err);
//         return;
//     }
// }

// Más adelante en tu código cuando quieras controlar el LED
// void set_led(uint8_t level) {
//     gpio_set_level(CAM_PIN_LED, level);
// }




// void send_to_cloud(const uint8_t* data, size_t length) {
//     // Log antes de iniciar la configuración del cliente HTTP
//     ESP_LOGI("send_to_cloud", "Configurando cliente HTTP para enviar datos a la nube...");

//     esp_http_client_config_t config = {
//         .url = GOOGLE_CLOUD_URL
//     };
//     esp_http_client_handle_t client = esp_http_client_init(&config);

//     esp_http_client_set_method(client, HTTP_METHOD_POST);
//     esp_http_client_set_header(client, "Content-Type", "application/octet-stream");

//     // Log antes de enviar los datos
//     ESP_LOGI("send_to_cloud", "Enviando datos a la nube...");

//     esp_http_client_set_post_field(client, (const char*)data, length);
//     esp_err_t err = esp_http_client_perform(client);

//     if (err == ESP_OK) {
//         ESP_LOGI("HTTP_CLIENT", "HTTP POST Status = %d, content_length = %" PRIu64,
//                  esp_http_client_get_status_code(client),
//                  esp_http_client_get_content_length(client));
//         // Log después de una carga exitosa
//         ESP_LOGI("send_to_cloud", "Imagen subida con éxito al bucket de GCP.");
//     } else {
//         ESP_LOGE("HTTP_CLIENT", "HTTP POST request failed: %s", esp_err_to_name(err));
//         // Log en caso de fallo en la carga
//         ESP_LOGE("send_to_cloud", "Falló la subida de la imagen al bucket de GCP.");
//     }

//     // Limpieza del cliente HTTP
//     esp_http_client_cleanup(client);
// }


