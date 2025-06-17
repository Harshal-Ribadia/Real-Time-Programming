// Do not remove the include below
#include "PrelimTask2.h"
#include "Pwm.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

const int external_led_1 = 8;
const int external_led_2 = 12;
const int external_led_3 = 13;
const int external_led_4 = 10;

const int pulse_width_out = 11;
const int analog_input_pin = A0;
int PWM_REG = 0;

void InitPWM()
{
	PWM_REG = 153;
	analogWrite(pulse_width_out, PWM_REG);
}

void setup()
{
	pinMode(external_led_1, OUTPUT);
	pinMode(external_led_2, OUTPUT);
	pinMode(external_led_3, OUTPUT);
	pinMode(external_led_4, OUTPUT);
	pinMode(pulse_width_out, OUTPUT);
	pinMode(analog_input_pin, INPUT);
	Serial.begin(115200);

	xTaskCreate(tBlinkLED, "Blink", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(tLevelControl, "Level",configMINIMAL_STACK_SIZE , NULL, 3, NULL);
	xTaskCreate(tMotortask, "Motor", configMINIMAL_STACK_SIZE , NULL, 3, NULL );
	InitPWM();
	vTaskStartScheduler(); //start scheduling
}

void loop()
{
	// Nothing to be added in this section;
}
