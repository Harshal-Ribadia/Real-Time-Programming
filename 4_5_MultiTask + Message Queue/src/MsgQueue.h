
#ifndef SRC_MSGQUEUE_H_
#define SRC_MSGQUEUE_H_

#include "mcommon.h" // commons for all modules
#include "uart.h"
extern uint16_t delay3;
extern xQueueHandle xQueueRX;


extern void uart_recieve(char);

void Init_MessageQueue();
void tControlTask(void *pvParameters);


#endif /* SRC_MSGQUEUE_H_ */
