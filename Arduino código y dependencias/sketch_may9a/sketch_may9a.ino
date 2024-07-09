#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "quirc.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

WebServer server(80);
TaskHandle_t QRCodeReader_Task;

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_VERDE_PIN     2   // Nuevo pin para el LED verde
#define LED_ROJO_PIN     15   // Nuevo pin para el LED rojo

struct QRCodeData {
  bool valid;
  int dataType;
  uint8_t payload[1024];
  int payloadLen;
};

struct quirc *q = NULL;
uint8_t *image = NULL;  
camera_fb_t * fb = NULL;
struct quirc_code code;
struct quirc_data data;
quirc_decode_error_t err;
struct QRCodeData qrCodeData;  
String QRCodeResult = "";
const char* ssid = "🦁 domin_martin";
const char* password = "12345678";
// const char* ssid = "MOVISTAR_9BFD";
// const char* password = "fPfNoTQfcTBwCfHGX4qX";
String lastPayload = "";
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 30000;
bool newQRCodeDetected = false;

void connectToWiFi();
void QRCodeReader(void * pvParameters);
void sendPayload(const String& payload);
void dumpData(const struct quirc_data *data);

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Iniciando configuración...");
  connectToWiFi();

  pinMode(LED_VERDE_PIN, OUTPUT);  // Configurar el pin del LED verde como salida
  pinMode(LED_ROJO_PIN, OUTPUT);   // Configurar el pin del LED rojo como salida
  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_ROJO_PIN, LOW);

  Serial.println("Configurando e inicializando la cámara...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  #if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  #endif

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al inicializar la cámara. Error:  0x%x", err);
    ESP.restart();
  }
  
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  
  Serial.println("Configuración e inicicialización de la cámara realizados de forma satisfactoria.");
  Serial.println();
  
  xTaskCreatePinnedToCore(
             QRCodeReader,
             "QRCodeReader_Task",
             10000,
             NULL,
             1,
             &QRCodeReader_Task,
             0);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
    unsigned long currentTime = millis();

    if (newQRCodeDetected && (QRCodeResult != lastPayload || currentTime - lastSendTime >= sendInterval) && QRCodeResult != "") {
      HTTPClient http;
      http.begin("https://us-central1-tfg-borjadominguez.cloudfunctions.net/qr_check");
      http.addHeader("Content-Type", "application/json");

      String payloadJson = "{\"action\":\"comparePayload\",\"payload\":\"" + QRCodeResult + "\"}";
      int httpResponseCode = http.POST(payloadJson);

      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);

      if (httpResponseCode > 0) {
        if (response == "success") {
          Serial.println("Access Granted!");
          digitalWrite(LED_VERDE_PIN, HIGH);  // Encender el LED verde
          delay(3000);                        // Mantener el LED encendido durante 3 segundos
          digitalWrite(LED_VERDE_PIN, LOW);   // Apagar el LED verde
        } else if (response == "incorrect") {
          Serial.println("Access Denied!");
          digitalWrite(LED_ROJO_PIN, HIGH);  // Encender el LED rojo
          delay(3000);                       // Mantener el LED encendido durante 3 segundos
          digitalWrite(LED_ROJO_PIN, LOW);   // Apagar el LED rojo
        } else {
          Serial.println("Respuesta desconocida del servidor.");
        }

        lastPayload = QRCodeResult;
        lastSendTime = currentTime;
        newQRCodeDetected = false;
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
  }
  delay(100);
}

void connectToWiFi() {
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void QRCodeReader(void * pvParameters) {
  Serial.println("QRCodeReader is ready.");
  Serial.print("QRCodeReader running on core ");
  Serial.println(xPortGetCoreID());
  Serial.println();

  while (1) {
    q = quirc_new();
    if (q == NULL) {
      Serial.print("Can't create quirc object\r\n");  
      continue;
    }

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      continue;
    }   

    quirc_resize(q, fb->width, fb->height);
    image = quirc_begin(q, NULL, NULL);
    memcpy(image, fb->buf, fb->len);
    quirc_end(q);

    int count = quirc_count(q);
    if (count > 0) {
      quirc_extract(q, 0, &code);
      err = quirc_decode(&code, &data);

      if (err) {
        Serial.println("Decoding FAILED: Error en la decodificación del QR code.");
      } else {
        String decodedResult = (const char *)data.payload;
        if (decodedResult != QRCodeResult) {
          Serial.println("Decoding successful:");
          dumpData(&data);
          QRCodeResult = decodedResult;  // Actualiza QRCodeResult después de la decodificación exitosa y diferente
          newQRCodeDetected = true;  // Set the flag for new QR code detection
        } else {
          Serial.println("QR code detectado, pero es el mismo que el anterior.");
        }
      } 
      Serial.println();
    }

    esp_camera_fb_return(fb);
    fb = NULL;
    image = NULL;  
    quirc_destroy(q);
  }
}

void dumpData(const struct quirc_data *data) {
  Serial.printf("Version: %d\n", data->version);
  Serial.printf("ECC level: %c\n", "MLHQ"[data->ecc_level]);
  Serial.printf("Mask: %d\n", data->mask);
  Serial.printf("Length: %d\n", data->payload_len);
  Serial.printf("Payload: %s\n", data->payload);

  QRCodeResult = (const char *)data->payload;
}
