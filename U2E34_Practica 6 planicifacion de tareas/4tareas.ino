#include <LiquidCrystal.h> //libreria para pantalla
#include <Wire.h> //librerias para mpu
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h> //MPU

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

// Configuración del LCD
LiquidCrystal lcd(25, 26, 16, 17, 18, 19); //VER SI SE PUEDEN CAMBIAR PINES PARA PODER COCTAR EL MPU sin problema

// Definición de pines
static const int led = 15;       // LED controlado por Tarea 1
static const int boton = 4;     // Botón para iniciar Tarea 1
static const int led2 = 5;       // LED controlado por Tarea 3
static const int boton2 = 23;    // Botón para iniciar Tarea 3
static const int led3 = 2;     // Botón para iniciar Tarea 5
static const int boton3 = 14;    // Botón para iniciar Tarea 6, MPU, cambiar pin

// Configuracion del MPU6050
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
volatile bool registroActivo = false;  // Bandera para iniciar/detener conteo
unsigned long tiempos[6] = {0}; // Tiempos en cada cara (6 caras del cubo)
unsigned long inicioTiempo = 0;
int j;
int caraActual = -1; 

// Variables compartidas
static SemaphoreHandle_t mutex;
volatile char tar ;



// TAREA 1: Si el botón está presionado, comienza secuencia
void Tarea1(void *parameter) {
  while (1) {
    if (tar == 'A') { // Solo se ejecuta si se seleccionó la tarea
      if (digitalRead(boton) == LOW) { // Verificar si el botón está presionado
        // Encender el LED por 3 segundos mientras el botón está presionado
        while (tar == 'A' && digitalRead(boton) == LOW) {
          digitalWrite(led, HIGH);
          vTaskDelay(3000 / portTICK_PERIOD_MS); // Esperar 3 segundos

          // Parpadeo mientras el botón siga presionado
          while (tar == 'A' && digitalRead(boton) == LOW) {
            digitalWrite(led, HIGH);
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Encendido 1 segundo
            digitalWrite(led, LOW);
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Apagado 1 segundo
          }
        }
        // Apagar el LED si el botón se suelta
        digitalWrite(led, LOW);
      } else {
        // Asegurarse de que el LED esté apagado si el botón no está presionado
        digitalWrite(led, LOW);
      }
    } else {
      // Apagar el LED si la tarea no está activa
      digitalWrite(led, LOW);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Reducir carga del bucle
  }
}

// TAREA 2: Simular señal de 0 a 3.3v, leer ADC y mostrar en LCD.
void Tarea2(void *parameter) {
  while (1) {
    if (tar == 'B') { // Solo se ejecuta si se seleccionó la tarea
      if (xSemaphoreTake(mutex, portMAX_DELAY)) { // Entrar a sección crítica
        int adcValue = analogRead(A0);            // Leer el ADC (conectar señal al pin A0)
        float voltage = (adcValue / 4095.0) * 3.3; // Convertir a voltaje (asumiendo 12 bits)

        // Mostrar en la pantalla LCD
        lcd.setCursor(0, 0);
        lcd.print("ADC: ");
        lcd.print(voltage, 2); // Mostrar voltaje con 2 decimales
        lcd.print("V    ");
        xSemaphoreGive(mutex); // Salir de la sección crítica
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS); // Actualizar cada 500 ms
  }
}

// TAREA 3: Parpadeo infinito de otro LED al presionar botón
void Tarea3(void *parameter) {
  while (1) {
    if (tar == 'C') { // Solo se ejecuta si se seleccionó la tarea
      if (digitalRead(boton2) == LOW) { // Verificar si el botón está presionado
        // Parpadeo infinito mientras el botón esté presionado
        while (tar == 'C' && digitalRead(boton2) == LOW) {
          digitalWrite(led2, HIGH);
          vTaskDelay(1000 / portTICK_PERIOD_MS); // Encendido 1 segundo
          digitalWrite(led2, LOW);
          vTaskDelay(1000 / portTICK_PERIOD_MS); // Apagado 1 segundo
        }
        // Asegurarse de apagar el LED cuando el botón se suelta
        digitalWrite(led2, LOW);
      } else {
        // Asegurarse de que el LED esté apagado si el botón no está presionado
        digitalWrite(led2, LOW);
      }
    } else {
      // Apagar el LED si la tarea no está activa
      digitalWrite(led2, LOW);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Reducir carga del bucle
  }
}

// Tarea5: brillo gradual de led
void Tarea5(void *parameter) {
  while (1) {
    if (tar == 'D') {
    for(int i=0;i<256;i++){
      analogWrite(led3, i);
      vTaskDelay(j / portTICK_PERIOD_MS);
    }
    for(int i=255;i>0;i--){
      analogWrite(led3, i);
      vTaskDelay(j / portTICK_PERIOD_MS);
    }
  }
  vTaskDelay(100 / portTICK_PERIOD_MS); // Reducir carga del bucle
  }
}

//Tarea 6. MPU cubo
void Tarea6 (void *pvParameters) {
  while (1) {
    if (tar == 'E') { // Solo se ejecuta si se seleccionó la tarea
    // Gestion del boton para iniciar/detener conteo
    if (digitalRead(boton3) == LOW) {
      registroActivo = !registroActivo;
      if (!registroActivo) {
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
          if (caraActual != -1) {
            tiempos[caraActual] += millis() - inicioTiempo;
            caraActual = -1;
          }
          xSemaphoreGive(mutex);
        }
        Serial.println("Conteo detenido. Enviando resultados...");
        // Enviar resultados por UART
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
          Serial.println("Registro de tiempos en cada cara del cubo:");
          for (int i = 0; i < 6; i++) {
            Serial.print("Cara "); Serial.print(i); Serial.print(": ");
            Serial.print(tiempos[i] / 1000.0, 2);
            Serial.println(" segundos");
          }
          xSemaphoreGive(mutex);
        }
      } else {
        inicioTiempo = millis();
        Serial.println("Conteo iniciado...");
      }
    }
      vTaskDelay(500 / portTICK_PERIOD_MS); // Antirebote
    }

    // Deteccion de caras y registro del tiempo
    if (registroActivo) {
      if (xSemaphoreTake(mutex, portMAX_DELAY)) {
        mpu.getEvent(&a, &g, &temp);
        int nuevaCara = detectarCara(a.acceleration.x, a.acceleration.y, a.acceleration.z);

        if (nuevaCara != caraActual && nuevaCara != -1) {
          if (caraActual != -1) {
            tiempos[caraActual] += millis() - inicioTiempo; // Registrar tiempo en la cara actual
          }
          inicioTiempo = millis();
          caraActual = nuevaCara;
          lcd.setCursor(0, 0);
          lcd.print("Cara actual: "); lcd.println(caraActual);
        }
        xSemaphoreGive(mutex);
      }
    }
    vTaskDelay(150 / portTICK_PERIOD_MS); // Espera para reducir carga de CPU
  }
}


// TAREA 4: por UART seleccionar una de las 3 tareas
void Tarea4(void *parameter) {
  while (1) {
    
    if (Serial.available() > 0) {
      char tar2 = Serial.read(); // Leer tarea seleccionada

      int L2=tar2; // variable auxiliar para la detección de salto de linea.
      int L;  // variable auxiliar para el delay de la tarea 5.
      if(L2!=10){// Si el la variable auxiliar no es un salto de línea en ASCII (10), entonces cambia su valor de delay.
        L=L2-48; // 48 es el número ASCII donde comienza el 0.
      }else{// Si la variable auxiliar es un salto de linea, se mantiene tal cual estaba antes.
        L=L;
      }
      if (tar2 == 'A'||tar2 =='B'||tar2 =='C'||tar2 =='D' || tar2 == 'E') {  // Validar tarea
        tar = tar2;                 // Asignar tarea seleccionada
        lcd.setCursor(0, 1);
        lcd.print("Tarea: ");
        lcd.print(tar);
        lcd.print("   ");
        lcd.setCursor(0, 0);
        lcd.print("                ");
      }
      Serial.println(L);
          if (L == 0 ){
            j=j;
          }else{
            j=L;
          
        }
        
      }
      vTaskDelay(100 / portTICK_PERIOD_MS); // Reducir carga del bucle
    }
  }




void setup() {
  // Configurar pines como salidas
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  

  // Configurar botones como entradas
  pinMode(boton, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);

  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.print("Iniciando...");

  // Inicializar comunicación serial
  Serial.begin(115200);
  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar el MPU6050. Verifique la conexion!");
    while (1);
  }
  Serial.println("MPU6050 listo!");
  
  // Crear Mutex
  mutex = xSemaphoreCreateMutex();

  // Crear tareas
  xTaskCreatePinnedToCore(Tarea1, "Tarea1", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Tarea2, "Tarea2", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Tarea3, "Tarea3", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Tarea4, "Tarea4", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(Tarea5, "Tarea5", 1024, NULL, 1, NULL,app_cpu);
  xTaskCreatePinnedToCore(Tarea6, "Tarea6", 8000, NULL, 1, NULL,app_cpu);
}
int detectarCara(float x, float y, float z) {
  if (z >= 8) return 0;   // Cara Z positiva
  if (z <= -8) return 1;  // Cara Z negativa
  if (y >= 8) return 2;   // Cara Y positiva
  if (y <= -8) return 3;  // Cara Y negativa
  if (x >= 8) return 4;   // Cara X positiva
  if (x <= -8) return 5;  // Cara X negativa
  return -1;             // Sin cara detectada
}
void loop() {
  // El loop no se utiliza en este código
}

// Funcion para detectar la cara actual basada en los ejes
