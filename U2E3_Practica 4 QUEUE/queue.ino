#if CONFIG_FREERTOS_UNICORE
    #define APP_CPU 0
#else
    #define APP_CPU 1
#endif

static const uint8_t msg_queue_len = 5;
static QueueHandle_t queue1;
static QueueHandle_t queue2;

// Tarea A: envía mensajes a Queue 2 y recibe de Queue 1
void TareaA(void *parameters){
  int recive_item;
  int envia_item = 0; //comienza en 0
  while (1){
    // Intenta recibir un mensaje de Queue 2
    if (xQueueReceive(queue2, (void*)&recive_item, 0) == pdTRUE){
      Serial.print("Tarea A recibió: ");
      Serial.println(recive_item);
      envia_item=recive_item;
    }
    
    // Envía un mensaje a Queue 1
    if (xQueueSend(queue1, (void*)&envia_item, 10) != pdTRUE){
      Serial.println("Queue 2 FULL desde Tarea A");
    } else {
      Serial.print("Tarea A envió: ");
      Serial.println(envia_item);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Tarea B: envía mensajes a Queue 1 y recibe de Queue 2
void TareaB(void *parameters){
  int recive_item;
  int envia_item = 1;  // Comienza en un valor diferente para diferenciar
  while (1){
    // Intenta recibir un mensaje de Queue 1
    if (xQueueReceive(queue1, (void*)&recive_item, 0) == pdTRUE){
      Serial.print("Tarea B recibió: ");
      Serial.println(recive_item);
    }

    // Envía un mensaje a Queue 2
    if (xQueueSend(queue2, (void*)&envia_item, 10) != pdTRUE){
      Serial.println("Queue 1 FULL desde Tarea B");
    } else {
      Serial.print("Tarea B envió: ");
      envia_item=recive_item+1;
      Serial.println(envia_item);
      //envia_item=0;
      recive_item=0;
    }


    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup(){
  Serial.begin(115200); 
  Serial.println("mensaje de inicio");  // Mensaje de inicio
  delay(500);

 
  queue1 = xQueueCreate(msg_queue_len, sizeof(int));
  queue2 = xQueueCreate(msg_queue_len, sizeof(int));
  
  xTaskCreatePinnedToCore(TareaA, "Tarea A", 1024, NULL, 1, NULL, APP_CPU);
  xTaskCreatePinnedToCore(TareaB, "Tarea B", 1024, NULL, 1, NULL, APP_CPU);
}


void loop(){
}
