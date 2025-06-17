################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../base_app/dia_comm4.c \
../base_app/uart.c 

OBJS += \
./base_app/dia_comm4.o \
./base_app/uart.o 

C_DEPS += \
./base_app/dia_comm4.d \
./base_app/uart.d 


# Each subdirectory must supply rules for building sources it contributes
base_app/%.o: ../base_app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


