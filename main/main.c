// main.c

#include "headers.h"
SemaphoreHandle_t wifi_semaphore = NULL; // Definición real

// URL del Google Cloud Storage donde se almacenarán las fotos
#define GOOGLE_CLOUD_URL "https://storage.googleapis.com/bucket-bdominguez-pei"

// Configuración de pines para la placa AI-Thinker
// #define CAM_PIN_PWDN    -1 // Power Down pin no es usado
// #define CAM_PIN_RESET    1 // Reset pin es usado (RST en la placa)
// #define CAM_PIN_XCLK    21
// #define CAM_PIN_SIOD    26
// #define CAM_PIN_SIOC    27
// #define CAM_PIN_D7      35
// #define CAM_PIN_D6      34
// #define CAM_PIN_D5      39
// #define CAM_PIN_D4      36
// #define CAM_PIN_D3      19
// #define CAM_PIN_D2      18
// #define CAM_PIN_D1       5
// #define CAM_PIN_D0       4
// #define CAM_PIN_VSYNC   25
// #define CAM_PIN_HREF    23
// #define CAM_PIN_PCLK    22
// #define CAM_PIN_LED      4 // Pin D0 como LED

// #define PWDN_GPIO_NUM     -1
// #define RESET_GPIO_NUM    -1
// #define XCLK_GPIO_NUM     0
// #define SIOD_GPIO_NUM     26
// #define SIOC_GPIO_NUM     27

// #define Y9_GPIO_NUM       35
// #define Y8_GPIO_NUM       34
// #define Y7_GPIO_NUM       39
// #define Y6_GPIO_NUM       36
// #define Y5_GPIO_NUM       21
// #define Y4_GPIO_NUM       19
// #define Y3_GPIO_NUM       18
// #define Y2_GPIO_NUM        5
// #define VSYNC_GPIO_NUM    25
// #define HREF_GPIO_NUM     23
// #define PCLK_GPIO_NUM     22

// Definiciones de pines para la cámara ESP32-CAM (AI-Thinker)
#define BOARD_WROVER_KIT 1

// WROVER-KIT PIN Map
#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
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
        .pin_pwdn = PWDN_GPIO_NUM,
        .pin_reset = RESET_GPIO_NUM,
        .pin_xclk = XCLK_GPIO_NUM,
        .pin_sccb_sda = SIOD_GPIO_NUM,
        .pin_sccb_scl = SIOC_GPIO_NUM,
        .pin_d7 = Y9_GPIO_NUM,
        .pin_d6 = Y8_GPIO_NUM,
        .pin_d5 = Y7_GPIO_NUM,
        .pin_d4 = Y6_GPIO_NUM,
        .pin_d3 = Y5_GPIO_NUM,
        .pin_d2 = Y4_GPIO_NUM,
        .pin_d1 = Y3_GPIO_NUM,
        .pin_d0 = Y2_GPIO_NUM,
        .pin_vsync = VSYNC_GPIO_NUM,
        .pin_href = HREF_GPIO_NUM,
        .pin_pclk = PCLK_GPIO_NUM,
        .xclk_freq_hz = 20000000,
        .pixel_format = PIXFORMAT_JPEG, // Selecciona el formato de los píxeles.
        .frame_size = FRAMESIZE_UXGA, // Selecciona el tamaño de la imagen capturada.
        .jpeg_quality = 12, // Calidad de compresión JPEG.
        .fb_count = 1, // Número de frame buffers a usar.
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


