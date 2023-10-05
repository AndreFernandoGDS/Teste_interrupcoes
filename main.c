#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


QueueHandle_t interruptQueue; //handle queue

#define = button    2
#define = Led       2
#define = portão_1 17
#define = portão_2 18



static void IRAM_ATTR gpio4_isr_handler(void *params) // cria a função de interrupçao ('static void IRAM_ATTR' padrão da função ISR)
{
	int pino = (int)params; //converte o ponteiro do parametro para inteiro e atribuia var. pino (será o numero do GPIO acionada na ISR)
	xQueueSendFromISR(interruptQueue, &pino, NULL);
}


void taskTrataISR(void *params)
{
	int pino;
	int contador = 0;

	while(true)

//De-Bouncing
	if(xQueueReceive(interruptQueue, &pino, portMAX_DELAY))
	    {
	      // De-bouncing
	      int estado = gpio_get_level(pino);
	      if(estado == 1)
	      {
	        gpio_isr_handler_remove(pino);
	        while(gpio_get_level(pino) == estado)
	        {
	          vTaskDelay(50 / portTICK_PERIOD_MS);
	        }
	        // Habilitar novamente a interrupção
	        vTaskDelay(50 / portTICK_PERIOD_MS);
	        gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
	      }
	    }
}
