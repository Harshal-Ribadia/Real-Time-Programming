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

2. PWM Motor Control with FreeRTOS on Arduino Uno
This project demonstrates how to generate and use Pulse Width Modulation (PWM) signals to control a motor model (RC low-pass filter), while leveraging FreeRTOS to manage tasks in a real-time environment on an Arduino Uno.

📋 Description
The project explores how PWM works and how it can be used to control the speed of a motor (or simulate one using an RC low-pass filter).

FreeRTOS, a lightweight real-time operating system, is used to schedule tasks instead of traditional time-slice scheduling.

Multiple tasks (such as setting PWM values, monitoring input, or adjusting duty cycle) are run in a multi-tasking environment using FreeRTOS features such as vTaskDelay(), priorities, and task creation.

The RC filter acts as a first-order low-pass filter, converting the digital PWM signal into an analog-like voltage, simulating motor behavior.

This setup allows for analysis of the response characteristics and timing behavior of PWM-controlled systems in both hardware and software.

🔧 Features
PWM signal generation using Arduino analogWrite() or direct timer register configuration.

Motor model simulated using an RC circuit to visualize filtering behavior of PWM.

FreeRTOS task creation, delays, and priority-based scheduling.

Comparison between delay() (blocking) and vTaskDelay() (non-blocking, RTOS-aware).

Discussion of jitter, CPU load, and resource usage with and without RTOS.

Practical application of theoretical concepts like time constants (τ), duty cycle calculation, and real-time scheduling.

📈 Educational Value
Understanding how PWM signals can be used to drive analog-like behavior in motors.

Experimenting with task scheduling algorithms: Fixed Time Slice (FTS), Fixed Priority Preemptive (FPP), etc.

Learning how RC low-pass filters smooth out PWM signals.

Gaining hands-on experience with FreeRTOS in an embedded system context.

Evaluating the trade-offs between bare-metal scheduling and RTOS-based implementations.

3.Ping-Pong Task Scheduling with FreeRTOS on Keil LPC4357 (ARM Cortex-M0/M4)
This real-time programming lab demonstrates the implementation of a Ping-Pong task scheme using FreeRTOS on the Keil LPC4357 evaluation board, which features a dual-core ARM Cortex-M0 and Cortex-M4 microcontroller. The project emphasizes Fixed Priority Preemptive (FPP) scheduling, semaphores, and task synchronization.

📋 Project Overview
Three FreeRTOS tasks are implemented:

Ping Task

Pong Task

Monitor Task (PrintTask)

The Ping and Pong tasks activate each other crosswise using binary semaphores, simulating a cooperative exchange.

The Monitor Task runs in parallel and measures the execution frequency of the Ping and Pong tasks, displaying this data on the onboard LCD using shared global variables (_d[]).

The project introduces students to:

The Eclipse + GCC for ARM development environment

FreeRTOS v7.3.0 basics (task creation, prioritization, semaphores)

Debugging via UART, JTAG, and LCD output

Programming on a dual-core ARM Cortex-M0/M4 system

🔧 System Components
Development Board: Keil MCB4300 (NXP LPC4357)

RTOS: FreeRTOS 7.3.0

Toolchain: Eclipse CDT, GCC for ARM, GDB, OpenOCD

Debugger: OpenOCD JTAG

Flasher: NXP Flash Magic

Libraries: NXP CMSIS (ARM Hardware Abstraction Layer)

🧠 Key Concepts
Ping-Pong Scheduling: Two tasks signal each other in turn via binary semaphores to create a controlled switching pattern.

FPP Scheduler: Tasks are prioritized using Fixed Priority Preemptive Scheduling; higher-priority tasks interrupt lower ones.

Dual-Core Coordination: While not directly programming both cores, the lab leverages LED indicators for M0 (LED 1) and M4 (LED 0) to illustrate dual-core behavior.

System Debugging:

_d[0] to _d[7] global variables used for debugging and display

LCD used to output count from the Monitor Task (_d[1])

🔍 Educational Objectives
Learn the architecture of NXP LPC4357 dual-core MCU (Cortex-M0 & Cortex-M4)

Understand task synchronization and inter-task communication in FreeRTOS

Measure real-time performance metrics (task frequency, timing)

Explore the differences between µC vs. ARM cores, RTOS vs. time-slice, and RTOS resource impact

4.Multitasking LED Control with FreeRTOS on LPC4357 (ARM Cortex-M0/M4)
This lab project demonstrates how to implement three concurrent FreeRTOS tasks for real-time control of multiple LEDs and interactive user input on the NXP LPC4357 dual-core ARM development board. The project is designed to deepen understanding of multitasking, real-time scheduling, and task interaction using FreeRTOS.

📋 Project Overview
This application builds on previous labs and introduces three FreeRTOS tasks:

tBlinking: Controls LED0 and LED1 to flash alternately at 5 Hz (every 100 ms).

tShifting: Creates a bidirectional running light using LED2 to LED7, with two LEDs active at a time. Updates every 50 ms, showing the position on _d[3].

tControl: Continuously monitors joystick position, and enables or disables tBlinking and tShifting based on the user’s input.

🔧 Features
FreeRTOS-based multitasking with:

Different task delays using vTaskDelay()

Task communication and conditional execution based on joystick input

LED animation patterns:

tBlinking alternates between two LEDs

tShifting moves two active LEDs across a 6-LED array, then reverses direction

Display feedback:

The current position of the running light is shown on the LCD via _d[3]

Code reuse:

Portions of task code and setup reused from earlier Ping-Pong and PWM lab exercises

🧠 Learning Objectives
Implement multiple independent periodic tasks with different timing constraints

Use FreeRTOS scheduling and delay mechanisms (vTaskDelay)

Monitor and respond to user inputs (joystick control) from a central task

Understand task interaction and control, conditional execution, and display updates

Practice code refactoring to modularize and reuse task behavior across projects

📦 Tools and Platform
Board: Keil MCB4300 with NXP LPC4357 ARM Cortex-M0/M4

RTOS: FreeRTOS 7.3.0

Toolchain: Eclipse CDT + GCC for ARM + GDB

Debugger: OpenOCD JTAG

Libraries: CMSIS for ARM, NXP peripheral drivers

Flashing Tool: NXP Flash Magic

5.ARM FreeRTOS: Task Synchronization with Message Queues
This project demonstrates task synchronization and communication using FreeRTOS Message Queues on the NXP LPC4357 ARM Cortex-M0/M4 dual-core development board. Building upon the previous Multitasking Lab, this exercise introduces inter-task messaging to manage and control real-time task behavior using joystick input and UART commands.

🎯 Project Objective
Implement a producer-consumer synchronization pattern using a FreeRTOS message queue:

tControl task produces control messages based on joystick inputs or UART commands

tWorker task consumes messages and takes appropriate action to suspend/resume or adjust LED task behavior

🧠 Learning Goals
Understand and apply FreeRTOS MessageQueue mechanisms

Design a task-controlled system using suspend/resume strategies

Integrate UART as an additional control interface

Modularize real-time code using handler and worker tasks

Reuse and refactor previous multitasking code for scalability
