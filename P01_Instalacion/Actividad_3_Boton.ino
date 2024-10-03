//Utilizamos solo core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

int static const LEDB = 14;
int static const LEDY = 12;
int static const boton = 15;
int status = 0;

//Tarea LED 01
void TareaLED01 (void *parameter){
  while (1){
    status = digitalRead(boton);
    if (status == HIGH){
      digitalWrite(LEDB, HIGH);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(LEDB, LOW);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
}else{
      digitalWrite(LEDB, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LEDB, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

//Tarea LED 02
void TareaLED02 (void *parameter){
  while (1){
    status = digitalRead(boton);
    if (status == HIGH){
      digitalWrite(LEDY, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LEDY, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
}else{
      digitalWrite(LEDY, HIGH);
      vTaskDelay(350 / portTICK_PERIOD_MS);
      digitalWrite(LEDY, LOW);
      vTaskDelay(350 / portTICK_PERIOD_MS);
    }
  }
}
void setup() {
  pinMode(boton, INPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDY, OUTPUT);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(TareaLED01,"LED 01",1024,NULL,1,NULL,app_cpu);
  xTaskCreatePinnedToCore(TareaLED02,"LED 02",1024,NULL,1,NULL,app_cpu);
}

void loop() {
}


