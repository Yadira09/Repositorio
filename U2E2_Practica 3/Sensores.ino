#include <LiquidCrystal.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//  pines del LCD
LiquidCrystal lcd(21, 22, 16, 17, 18, 19);

// Pines sensor ultrasónico
const int trigPin = 5;
const int echoPin = 23;

// Pin   sensor LDR 
const int ldrPin = 34;  

// Variables globales
SemaphoreHandle_t mutex;



// Función para medir la distancia con el sensor ultrasónico
float medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Función para leer el valor del LDR
int leerLuz() {
  return analogRead(ldrPin);  // Leer valor del LDR
}

// Tarea para  el sensor ultrasónico
void TareaUltrasonico(void *parameters) {
  const char *tarea = (const char *)parameters;
  
  while (1) {
    float distancia = medirDistancia();
    
    // Sección crítica: actualizar la pantalla
    if (xSemaphoreTake(mutex, 0) == pdTRUE) {
      lcd.setCursor(0, 0);
      lcd.print("Distancia: ");
      lcd.print(distancia);
      lcd.print(" cm   ");  // Espacios para limpiar
      Serial.print(tarea);
      Serial.println(" actualiza distancia.");
      
      xSemaphoreGive(mutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

// Tarea para leer el valor del sensor LDR y actualizar la pantalla
void TareaLuz(void *parameters) {
  const char *tarea = (const char *)parameters;
  
  while (1) {
    int luz = leerLuz();
    
    // Sección crítica: actualizar la pantalla
    if (xSemaphoreTake(mutex, 0) == pdTRUE) {
      lcd.setCursor(0, 1);
      lcd.print("Luz: ");
      lcd.print(luz);
      lcd.print("     ");  // Espacios para limpiar
      Serial.print(tarea);
      Serial.println(" actualiza nivel de luz.");
      
      xSemaphoreGive(mutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera 1 segundo
  }
}

void setup() {
  // Iniciar el LCD
  lcd.begin(16, 2);  // Configurar tamaño del LCD (16 columnas y 2 filas)
  lcd.setCursor(0, 0);
  

  // Iniciar serial para depuración
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  // Inicializar el sensor ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Crear mutex
  mutex = xSemaphoreCreateMutex();

  // Crear tareas
  xTaskCreatePinnedToCore(TareaUltrasonico, 
                          "Tarea Ultrasonico", 
                          1024, 
                          (void*)"Tarea Ultrasonico", 
                          1, 
                          NULL, 
                          app_cpu);
                          
  xTaskCreatePinnedToCore(TareaLuz, 
                          "Tarea Luz", 
                          1024, 
                          (void*)"Tarea Luz", 
                          1, 
                          NULL, 
                          app_cpu);
}

void loop() {
 
}
