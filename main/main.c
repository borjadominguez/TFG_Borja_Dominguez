// main.c

#include "headers.h"
SemaphoreHandle_t wifi_semaphore = NULL; // Definición real

// URL del Google Cloud Storage donde se almacenarán las fotos
#define GOOGLE_CLOUD_URL "https://storage.googleapis.com/bucket-bdominguez-pei"

// Definiciones de pines para la cámara ESP32-CAM (AI-Thinker)
#define BOARD_ESP32CAM_AITHINKER

#ifdef BOARD_ESP32CAM_AITHINKER
#define CAM_PIN_PWDN -1 // Power down pin no utilizado
#define CAM_PIN_RESET -1 // Software reset se realizará, no se necesita pin físico
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#endif

static const char *TAG = "example:take_picture";


// Prototipo de función para inicializar la cámara (si es necesario)


void app_main(void) {
    // Inicialización de NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Creación de semáforo para WiFi
    wifi_semaphore = xSemaphoreCreateBinary();
    if (wifi_semaphore == NULL) {
        ESP_LOGE("app_main", "Falló la creación del semáforo de WiFi.");
        return; // Detiene la ejecución si no se puede crear el semáforo
    }

    // Inicialización de la cámara y WiFi
    camera_init();
    wifi_setup();

    // Bucle principal de la aplicación
    // while (true) {
    //     ESP_LOGI("app_main", "Ejecutando en el bucle principal...");
    //     vTaskDelay(pdMS_TO_TICKS(10000)); // Espera de 10 segundos

    //         int count = 10;                            // Contador para la cantidad de veces que se ejecutará el bucle
    //         while (count > 0) {
    //             ESP_LOGI("app_main", "Intentando tomar una foto...");
    //             camera_fb_t* fb = esp_camera_fb_get(); // Captura una imagen con la cámara
    //             if (!fb) {                             // Verifica si la captura fue exitosa. Si fb es NULL, significa que hubo un error al capturar la imagen.
    //                 ESP_LOGE("CAMERA", "Error en la captura de imagen");
    //             } else {                               // Si la captura fue exitosa, llama a la función send_to_cloud()
    //                 ESP_LOGI("app_main", "Imagen tomada con éxito.");
    //                 send_to_cloud(fb->buf, fb->len);   // Envía los datos de la imagen capturada a Google Cloud Storage (Bucket)
    //                 esp_camera_fb_return(fb);          // Devuelve el búfer de la imagen para que pueda ser reutilizado
    //             }

    //             vTaskDelay(pdMS_TO_TICKS(10000));      // Espera durante 10 segundos antes de la próxima captura

    //             count -= 1;                            // Decrementa el contador
    //         }
    // }

}






// Definición de funciones

void camera_init() {
    camera_config_t config = {
        .pin_pwdn = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,
        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,
        //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
        .frame_size = FRAMESIZE_UXGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG.
        .jpeg_quality = 12, //0-63 lower number means higher quality
        .fb_count = 2,  //if more than one, i2s runs in continuous mode. Use only with JPEG
        .grab_mode = CAMERA_GRAB_LATEST, // Takes new frame when buffer is available
    
    };

    // Iniciar la cámara
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE("camera", "Camera init failed with error 0x%x", err);
    } else {
        ESP_LOGI("camera", "Camera initialized successfully");
    }
}












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


