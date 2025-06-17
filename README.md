# Real-Time-Programming

1.Fixed Time Slice Scheduler Using Timer Interrupts on Arduino Uno
This project demonstrates the implementation of a Fixed Time Slice (FTS) Scheduler on an Arduino Uno using the AVR ATmega328P microcontroller. It showcases how to configure timer interrupts to create a precise time base for simple real-time task scheduling.

📋 Description
Timer interrupts are used to create a consistent time base using one of the Arduino Uno's hardware timers.

An Interrupt Service Routine (ISR) is triggered at fixed intervals (the time slice, TTS) to simulate a basic real-time operating system scheduler.

A global counter (TTS_counter) is incremented in the ISR to keep track of time slices.

The FTS Scheduler function is called within the ISR and uses the counter value to determine which task function to execute (e.g., toggling an LED, reading a sensor, etc.).

Tasks are scheduled in a round-robin or time-sliced manner, ensuring each task runs at predictable and fixed intervals.

The system behavior (e.g., blinking LED) can be verified using an oscilloscope to observe the precise timing.

🔧 Features
Configurable Timer1 interrupt setup (CTC mode).

Simple real-time scheduling logic.

Deterministic task switching based on counter values.

Modular task implementation for extensibility.

📈 Use Case
Ideal for educational use in embedded systems, real-time systems, or microcontroller lab experiments where students learn the basics of:

Timer configuration in AVR

Real-time task scheduling

Interrupt handling
