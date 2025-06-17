################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PingPong.c \
../src/UartTX.c \
../src/app.c \
../src/main.c 

OBJS += \
./src/PingPong.o \
./src/UartTX.o \
./src/app.o \
./src/main.o 

C_DEPS += \
./src/PingPong.d \
./src/UartTX.d \
./src/app.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


