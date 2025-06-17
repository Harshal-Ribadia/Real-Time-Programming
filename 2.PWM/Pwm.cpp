#include <Arduino.h>
#include "Pwm.h"
#include <Arduino_FreeRTOS.h>

extern const int external_led_1 = 8;
extern const int external_led_2 = 12;
extern const int external_led_3 = 13;
extern const int pulse_width_out = 11;
extern const int external_led_4 = 10;
extern const int analog_input_pin = A0;

bool led_1_state = 0;
bool led_2_state = 0;
bool led_3_state = 0;

void tBlinkLED( void * pvParameters )
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for( ;; )
	{
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(500));
		led_1_state = !led_1_state;
		digitalWrite(external_led_1, led_1_state);
	}
}

void tLevelControl( void * pvParameters )
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
    int threshold = 40;
    int value = 0;

	for( ;; )
	{
		digitalWrite(external_led_4, HIGH);
		value = analogRead(analog_input_pin);
		digitalWrite(external_led_4, LOW);

		if (value > threshold)
		{
			digitalWrite(external_led_2, HIGH);

			led_2_state = true;
		}
		else
		{
			digitalWrite(external_led_2, LOW);

			led_2_state = false;

		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}

}

void tMotortask(void * pvParameters)
{
	uint8_t n = 0; // Initialization before the while so to prevent the declaration every loop iteration:-
	uint8_t DAC_values[4] = {0, 51, 128, 255}; // values correspond to 0 %,← 20 %, 50 %, 100 %
	uint8_t DAC_out;
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
		DAC_out = DAC_values[n]; // current input to low pass filter
		analogWrite(pulse_width_out, DAC_out);
		n ++;
		if(n >= 4)
		{
			n = 0;
		}
	}
}
