#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif
static const int led01=12;
static const int led02=14;
//Tarea1
void LED_1(void *parameter){
  while(1){
    digitalWrite(led01, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led01, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
//Tarea2
void LED_2(void *parameter){
  while(1){
    digitalWrite(led02, HIGH);
    vTaskDelay(350 / portTICK_PERIOD_MS);
    digitalWrite(led02, LOW);
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
void setup() {
  pinMode(led01, OUTPUT);
  pinMode(led02, OUTPUT);
  xTaskCreatePinnedToCore(LED_1,"Toggle1",1024,NULL,2,NULL,app_cpu); 
  xTaskCreatePinnedToCore(LED_2,"Toggle2",1024,NULL,1,NULL,app_cpu);
}
void loop() { 
}

