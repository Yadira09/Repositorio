#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static int shared_var = 0;
static SemaphoreHandle_t mutex;

// Definición de pines
static const int led_VA = 16;
static const int led_VB = 15;
static const int led_V1 = 14;
static const int led_V2 = 13;
static const int led_motor = 12;

static const int sp1B = 18; // Botón para pasar de VA a VB
static const int sp2B = 5; // Botón para pasar de VB a V1
static const int inicio = 19; // Botón para iniciar la secuencia

// Banderas de control de secuencia
bool vaCompleta = false;
bool vbCompleta = false;
bool v1Completa = false;
bool v2Completa = false;

// Estado anterior de los botones
bool inicioState = false;
bool sp1BState = false;
bool sp2BState = false;

// Función para detectar presiones de botón únicas (borde de subida)
bool detectButtonPress(int buttonPin, bool &previousState) {
  bool currentState = digitalRead(buttonPin);
  if (currentState && !previousState) {
    previousState = currentState;
    return true;
  }
  previousState = currentState;
  return false;
}

// TAREA 01. Control de la Válvula VA
void ValvulaA(void *parameter) {
  int local_var;
  while (1) {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) { // Solicitar el permiso para entrar a la sección crítica
      local_var = shared_var;
      local_var++;
      shared_var = local_var;

      if (detectButtonPress(inicio, inicioState)) {  // Detectar presionado del botón de inicio
        // Encender VA y apagar el resto
        digitalWrite(led_VA, HIGH);
        digitalWrite(led_VB, LOW);
        digitalWrite(led_V1, LOW);
        digitalWrite(led_V2, LOW);
        digitalWrite(led_motor, LOW);
        vaCompleta = true;  // Marcar que VA ha finalizado
      }

      xSemaphoreGive(mutex); // Liberar el Mutex
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Retraso para evitar un bucle rápido
  }
}

// TAREA 02. Control de la Válvula VB
void ValvulaB(void *parameter) {
  int local_var;
  while (1) {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) { // Solicitar el permiso para entrar a la sección crítica
      local_var = shared_var;
      local_var++;
      shared_var = local_var;

      if (vaCompleta && detectButtonPress(sp1B, sp1BState)) {  // Detectar presión de sp1B tras el fin de VA
        // Encender VB y apagar VA
        digitalWrite(led_VB, HIGH);
        digitalWrite(led_VA, LOW);
        vbCompleta = true;  // Marcar que VB ha finalizado
      }

      xSemaphoreGive(mutex); // Liberar el Mutex
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// TAREA 03. Control de la Válvula V1
void Valvula1(void *parameter) {
  int local_var;
  while (1) {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) { // Solicitar el permiso para entrar a la sección crítica
      local_var = shared_var;
      local_var++;
      shared_var = local_var;

      if (vbCompleta && detectButtonPress(sp2B, sp2BState)) {  // Detectar presión de sp2B después de VB
        // Encender V1 y apagar VB
        digitalWrite(led_V1, HIGH);
        digitalWrite(led_VB, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Mantener V1 encendida durante 1 segundo
      
        v1Completa = true;  // Marcar que V1 ha finalizado
      }

      xSemaphoreGive(mutex); // Liberar el Mutex
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// TAREA 04. Control de la Válvula V2
void Valvula2(void *parameter) {
  int local_var;
  while (1) {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) { // Solicitar el permiso para entrar a la sección crítica
      local_var = shared_var;
      local_var++;
      shared_var = local_var;

      if (v1Completa) {  // Solo si V1 ha finalizado
        // Encender V2 y apagar V1
        digitalWrite(led_V2, HIGH);
        digitalWrite(led_V1, LOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Mantener V2 encendida durante 1.5 segundos
        digitalWrite(led_V2, LOW);
        v2Completa = true;  // Marcar que V2 ha finalizado
        v1Completa = false; // Reiniciar V1
      }

      xSemaphoreGive(mutex); // Liberar el Mutex
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// TAREA 05. Control del Motor
void Motor(void *parameter) {
  int local_var;
  while (1) {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) { // Solicitar el permiso para entrar a la sección crítica
      local_var = shared_var;
      local_var++;
      shared_var = local_var;

      if (v2Completa) {  // Solo si V2 ha finalizado
        // Encender motor
        digitalWrite(led_motor, HIGH);
        digitalWrite(led_V2, LOW);  // Apagar V2 al iniciar el motor
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Mantener el motor encendido durante 1.5 segundos
        digitalWrite(led_motor, LOW);  // Apagar motor
        v2Completa = false;  // Reiniciar la secuencia
        // Reiniciar las banderas
        vaCompleta = false;
        vbCompleta = false;
        v1Completa = false;
      }

      xSemaphoreGive(mutex); // Liberar el Mutex
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Configurar pines como salidas
  pinMode(led_VA, OUTPUT);
  pinMode(led_VB, OUTPUT);
  pinMode(led_V1, OUTPUT);
  pinMode(led_V2, OUTPUT);
  pinMode(led_motor, OUTPUT);
  
  // Configurar botones como entradas
  pinMode(sp1B, INPUT);
  pinMode(sp2B, INPUT);
  pinMode(inicio, INPUT);
  
  // Crear Mutex antes de comenzar las tareas
  mutex = xSemaphoreCreateMutex();

  // Crear las tareas
  xTaskCreatePinnedToCore(ValvulaA, "VA", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(ValvulaB, "VB", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Valvula1, "V1", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Valvula2, "V2", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Motor, "Motor", 1024, NULL, 1, NULL, app_cpu);
}

void loop() {
  // No se necesita el loop en este código
}
