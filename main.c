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
