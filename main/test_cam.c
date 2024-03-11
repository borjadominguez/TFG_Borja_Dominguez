

#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Reemplaza con tus valores de configuración WiFi
const char* ssid = "MOVISTAR_9BFD";
const char* password = "fPfNoTQfcTBwCfHGX4qX";

void setup() {
  Serial.begin(115200);
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  
  // Configuración de la cámara (asegúrate de usar la configuración correcta para tu modelo de ESP32-CAM)
  camera_config_t config;
  // La configuración de la cámara se omite por brevedad. Configura según tu modelo específico.

  // Inicialización de la cámara
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Error al inicializar la cámara");
    return;
  }
}

void loop() {
  // Tomar una foto
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar la foto");
    return;
  }

  // Enviar la foto
  HTTPClient http;
  http.begin("https://storage.googleapis.com/bucket-bdominguez-pei"); // Cambia a tu URL
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);
  
  if (httpResponseCode > 0) {
    // Recibir la respuesta del servidor
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error en la solicitud: ");
    Serial.println(httpResponseCode);
  }

  // Finalizar
  http.end();
  esp_camera_fb_return(fb);
  delay(10000); // Espera por 10 segundos antes de la próxima foto
}
