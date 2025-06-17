
#include "Timeslice.h"
#include <Arduino.h>

unsigned int TimeCounter_ = 0;
const int E_RED_LED = 13;
const int E_GREEN_LED = 12;
const int E_LED = 8;

bool GREEN_STATE = 0;
bool RED_STATE = 0;
bool E_STATE = 0;


void _TimeSliceScheduler_()
{

	if((TimeCounter_ % 2) == 1)
	{
		digitalWrite(E_LED, HIGH);
		_ToggleTask_();
		digitalWrite(E_LED, LOW);
	}

	else if((TimeCounter_ % 1024) == 512)
	{
		_BlinkTask_();
	}

	TimeCounter_++;
}

void _ToggleTask_()
{
	GREEN_STATE = !GREEN_STATE;
	digitalWrite(E_GREEN_LED, GREEN_STATE);

}

void _BlinkTask_()
{
	RED_STATE = !RED_STATE;
	digitalWrite(E_RED_LED, RED_STATE);

}
