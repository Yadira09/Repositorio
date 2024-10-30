#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static const int led = 4;  

static TimerHandle_t segundos = NULL; 
static TimerHandle_t flash = NULL; 

int num_serial = 0;  //numeor ungresado al serial
int seg = 1; //contador

// Función para verificar si un número es primo
bool primo(int num) {
  if (num <= 1) return false; // Los números menores o iguales a 1 no son primos
  for (int i = 2; i * i <= num; i++) {
    if (num % i == 0) return false; // Si num es divisible por i, no es primo
  }
  return true;  // Si no se encontró ningún divisor, el número es primo
}

// Función para reiniciar el programa
void resetProgram() {
  // Restablece los valores de las variables
  seg = 1;
  num_serial = 0;
  
  //introducir datos
  Serial.println("Introduce un número:");
  while (Serial.available() == 0) {} // Espera hasta que haya datos disponibles
  num_serial = Serial.parseInt();  //ingresa numero entero 
  Serial.print("Número recibido: ");
  Serial.println(num_serial);

  // Reinicia el temporizador de segundos
  xTimerStart(segundos, portMAX_DELAY);
}


void TSegundos(TimerHandle_t xTimer) {
  if (seg > num_serial) {
    xTimerStop(segundos, 0);
    xTimerStop(flash, 0);
    digitalWrite(led, LOW); 
    
    // Reiniciar el programa después de alcanzar el número 
    resetProgram();
    return;
  }
  
  Serial.print("segundo: ");
  Serial.println(seg);
  
  if (primo(seg)) {
    // Si es primo, enciende el LED por un breve momento
    digitalWrite(led, HIGH);
    xTimerStart(flash, 0); // Inicia el flash timer para apagar el LED
  }
  
  seg++;
}

// Callback para el temporizador de flash
void TFlash(TimerHandle_t xTimer) {
  digitalWrite(led, LOW);  // Apaga el LED después del flash
  xTimerStop(flash, 0);  // Detiene el timer hasta el próximo número primo
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  Serial.println("Introduce un número:");
  while (Serial.available() == 0) {} // Espera hasta que haya datos disponibles
  num_serial = Serial.parseInt();
  Serial.print("Número  recibido: ");
  Serial.println(num_serial);

  // Configura el timer para contar segundos
  segundos = xTimerCreate(
    "segundos",             // Nombre del timer
    1000 / portTICK_PERIOD_MS,  // Intervalo de 1 segundo
    pdTRUE,                 // Auto-reload
    (void*)1,               // ID del timer
    TSegundos        // Callback de segundos
  );
  
  // Configura el timer para el flash del LED (100 ms flash)
  flash = xTimerCreate(
    "flash",                // Nombre del timer
    100 / portTICK_PERIOD_MS,  // Intervalo de 100 ms para un "flash" rápido
    pdFALSE,                // One-shot
    (void*)2,               // ID del timer
    TFlash           // Callback de flash
  );

  xTimerStart(segundos, portMAX_DELAY); // Empieza a contar segundos
}

void loop() {
  // El loop está vacío, ya que todo se maneja con timers
}
