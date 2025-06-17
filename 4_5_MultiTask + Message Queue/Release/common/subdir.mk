################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/ipc_app.c \
../common/ipc_msg.c \
../common/m0_ImageLoader.c \
../common/printf-stdarg.c \
../common/supervision.c \
../common/ws_strfun.c 

OBJS += \
./common/ipc_app.o \
./common/ipc_msg.o \
./common/m0_ImageLoader.o \
./common/printf-stdarg.o \
./common/supervision.o \
./common/ws_strfun.o 

C_DEPS += \
./common/ipc_app.d \
./common/ipc_msg.d \
./common/m0_ImageLoader.d \
./common/printf-stdarg.d \
./common/supervision.d \
./common/ws_strfun.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


