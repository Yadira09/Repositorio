//Utilizamos sólo core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

const char msg[]="Unidad Profesional Interdisciplinaria de Ingeniería Campus Zacatecas IPN";

int static const UART1 = 4;
int static const UART2 = 2;

//Tarea UART 01
void TareaUART01(void *parameter){
  int msg_len = strlen(msg);
  while(1){
    Serial.println();
    for(int i =0; i<msg_len; i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 02
void TareaUART02(void *parameter){
  while(1){
    digitalWrite(4, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(4, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("1");
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 03
void TareaUART03(void *parameter){
  while(1){
    Serial.println("2");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 04
void TareaUART04(void *parameter){
  while(1){
    Serial.println("3");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 05
void TareaUART05(void *parameter){
  while(1){
    Serial.println("4");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 06
void TareaUART06(void *parameter){
  while(1){
    Serial.println("5");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 07
void TareaUART07(void *parameter){
  while(1){
    digitalWrite(2, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(2, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("6");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
pinMode(4, OUTPUT);
pinMode(2, OUTPUT);
  Serial.begin(300);
  xTaskCreatePinnedToCore(
    TareaUART01,  //Función que se manda llamar
    "UART 01",  //Nombre de la tarea
    1024,         //Stack size
    NULL,         //
    2,            //Prioridades
    NULL,         //
    app_cpu);     //Core
    xTaskCreatePinnedToCore(TareaUART02, "UART 02", 1024, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(TareaUART03, "UART 03", 1024, NULL, 4, NULL, app_cpu);
    xTaskCreatePinnedToCore(TareaUART04, "UART 04", 1024, NULL, 3, NULL, app_cpu);
    xTaskCreatePinnedToCore(TareaUART05, "UART 05", 1024, NULL, 5, NULL, app_cpu);
    xTaskCreatePinnedToCore(TareaUART06, "UART 06", 1024, NULL, 7, NULL, app_cpu);
    xTaskCreatePinnedToCore(TareaUART07, "UART 07", 1024, NULL, 6, NULL, app_cpu);
}


void loop() {
}