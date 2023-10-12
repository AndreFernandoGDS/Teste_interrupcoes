#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


QueueHandle_t interruptQueue; //handle queue

#define button    4  //but, abrir
#define Led       2  //sinalisa ab/fech
#define portao_a 12  //motor abrir
#define portao_f 13  //motor fechar
#define FCA      16  //fim de curso aberto
#define FCF      17  // fim de curso fechado

int LIGADO = 1;
int DESLIGADO = 0;


enum estados {inicio, fechado, aberto, fechando, abrindo, parado_a, parado_f} estado;



static void IRAM_ATTR gpio_isr_handler(void *params) // cria a função de interrupçao
{
	int pino = (int)params; //converte o ponteiro do parametro para inteiro e atribuia var. pino (será o numero do GPIO acionada na ISR)
	xQueueSendFromISR(interruptQueue, &pino, NULL);
}


void taskTrataISR(void *params)
{
	int pino;

	while(true)
	{
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
				contador++;

				// Habilitar novamente a interrupção
				vTaskDelay(50 / portTICK_PERIOD_MS);
				gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
			  }
			}

			//Estrutura da Máquina de Estados
			switch (estado)

			case inicio:

			estado= fechando;
			break;


			case fechando:

			gpio_set_level (portao_f, LIGADO); //motor fechando ligado

			if (button)
			{
			estado = parado_f;
			}

			 if(FCF)
			 {
			 estado = fechado;
			 }
			break;


			case parado_f:

			gpio_set_level (portao_f, DESLIGADO);//motor desligado

			if (button)
			{
			estado=abrindo;
			}
			if(Temporisador)
			{
			estado=fechando;
			}
		    break;



			case fechado:

			gpio_set_level (portao_f, DESLIGADO);//motor desligado

			if (button)
			{
			estado=abrindo;
			}
			break;


			case abrindo:

			gpio_set_level (portao_f, LIGADO);//motor ligado abrindo
			if (button)
			{
			estado=parado_a;
			}
			if(FCA)
			{
			estado=aberto;
			}
			break;


			case parado_a:

			 gpio_set_level (portao_a, DESLIGADO);//motor desligado
			if (button) { estado=fechando;
			 }
			 if(Temporisador)
			 {
		      estado=fechando;
			 }
			 break;



			case aberto:

			gpio_set_level (portao_a, DESLIGADO);//motor desligado
			if (button)
			{
			estado=fechando;
			}
			if(Temporisador)
			{
			estado=fechando;
			}
			break;

	}
}


void app_main()
{

	  estado = inicio;


	 // Configuração dos GPIO
	  gpio_pad_select_gpio(Led);
	  gpio_pad_select_gpio(portao_a);
	  gpio_pad_select_gpio(portao_f);
	  gpio_pad_select_gpio(button);
	  gpio_pad_select_gpio(FCA);
	  gpio_pad_select_gpio(FCF);


	  // Configura GPIO entradas e saídas
	  gpio_set_direction(Led, GPIO_MODE_OUTPUT);
	  gpio_set_direction(portao_a, GPIO_MODE_OUTPUT);
	  gpio_set_direction(portao_f, GPIO_MODE_OUTPUT);
	  gpio_set_direction(button, GPIO_MODE_INPUT);
	  gpio_set_direction(FCA, GPIO_MODE_INPUT);
	  gpio_set_direction(FCF, GPIO_MODE_INPUT);




	  // Configura o resistor de Pulldown para entradas
	  gpio_pulldown_en(button);
	  gpio_pulldown_en(FCA);
	  gpio_pulldown_en(FCF);


	  // Desabilita Pull-up por segurança.
	  gpio_pullup_dis(button);
	  gpio_pullup_dis(FCA);
	  gpio_pullup_dis(FCF);

	  // Configura pino para interrupção
	  gpio_set_intr_type(button, GPIO_INTR_POSEDGE);
	  gpio_set_intr_type(FCA, GPIO_INTR_POSEDGE);
	  gpio_set_intr_type(FCF, GPIO_INTR_POSEDGE);



	  gpio_install_isr_service(0);
	  gpio_isr_handler_add(button, gpio_isr_handler, (void *) button);
	  gpio_isr_handler_add(FCA, gpio_isr_handler, (void *) FCA);
	  gpio_isr_handler_add(FCF, gpio_isr_handler, (void *) FCF);

	  filaDeInterrupcao = xQueueCreate(10, sizeof(int));//cria a fila
	  xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);//cria task principal




}
