################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup_code/cr_startup_lpc18xx43xx.c 

OBJS += \
./startup_code/cr_startup_lpc18xx43xx.o 

C_DEPS += \
./startup_code/cr_startup_lpc18xx43xx.d 


# Each subdirectory must supply rules for building sources it contributes
startup_code/%.o: ../startup_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


