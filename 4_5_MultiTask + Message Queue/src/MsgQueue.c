#include "msgQueue.h"
xQueueHandle xQueueRX;
void Init_MessageQueue()
{
	xQueueRX = xQueueCreate(8, sizeof(char));
}

char controlCharacter;
void tControlTask(void *pvParameters) // this is tWorker
{
	uint16_t joy;
	uint16_t btn;
	while (1)
	{
		joy = Joystick_GetStatus();
		btn = Buttons_GetStatus();
		switch (joy)
		{
		case (JOY_UP):
						controlCharacter= 'S';
		break;
		case (JOY_DOWN):
						controlCharacter= 's';
		break;
		case (JOY_RIGHT):
						if (btn== BUTTONS_BUTTON1){
							controlCharacter = '-';
						}
						else{
							controlCharacter= 'b';
						}
		break;
		case (JOY_LEFT):
				if (btn== BUTTONS_BUTTON1){
					controlCharacter = '+';
				}
				else{
					controlCharacter= 'B';
				}
		break;
		default:
			controlCharacter = NULL;
			break;
		}

		if (controlCharacter)
			xQueueSend(xQueueRX,(const void *) &controlCharacter, 0 );


		vTaskDelay(delay3);

	}
}

void uart_receive ( char c ) // called by UART ISR
{
	xQueueSendFromISR(xQueueRX, (const void *) &c, 0);
}
