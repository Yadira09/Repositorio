//Utilizamos sólo core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

const char msg1[]="Mensaje 1";
const char msg2[]="Mensaje 2";
const char msg3[]="Mensaje 3";
static TaskHandle_t tar1= NULL;
static TaskHandle_t tar2= NULL;
static TaskHandle_t tar3= NULL;
//Tarea UART 01
void TareaUART01(void *parameter){
  int msg_len = strlen(msg1);
  while(1){
    Serial.println();
    for(int i =0; i<msg_len; i++){
      Serial.print(msg1[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 02
void TareaUART02(void *parameter){
  int msg_len = strlen(msg2);
  while(1){
    Serial.println();
    for(int i =0; i<msg_len; i++){
      Serial.print(msg2[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea UART 03
void TareaUART03(void *parameter){
  int msg_len = strlen(msg3);
  while(1){
    Serial.println();
    for(int i =0; i<msg_len; i++){
      Serial.print(msg3[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void setup() {
  Serial.begin(300);
  xTaskCreatePinnedToCore(
    TareaUART01,  //Función que se manda llamar
    "UART 01",  //Nombre de la tarea
    1024,         //Stack size
    NULL,         //
    2,            //Prioridades
    &tar1,         //
    app_cpu);     //Core
    xTaskCreatePinnedToCore(TareaUART02,  "UART 02", 1024,  NULL, 1,&tar2,app_cpu);  
    xTaskCreatePinnedToCore(TareaUART03, "UART 03", 1024, NULL, 3, &tar3, app_cpu);
    
}

void loop() {
  for(int i =0; i<2; i++){
    vTaskSuspend(tar2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(tar2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
    vTaskDelete(tar1);
}
