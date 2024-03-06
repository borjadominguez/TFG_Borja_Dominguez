// headers.h

#include "esp32-libs.h"

// CONFIGURACIONES INICIALES
void wifi_setup(void);

// INCLUDES INICIALES
#include "esp32-libs.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"


//SEMAPHORE
extern SemaphoreHandle_t wifi_semaphore;
extern SemaphoreHandle_t sntp_semaphore;
extern SemaphoreHandle_t mqtt_semaphore;
extern SemaphoreHandle_t dtct_semaphore;


