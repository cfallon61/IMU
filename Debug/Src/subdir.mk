################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/daq.c \
../Src/l3gd20h.c \
../Src/lsm303d.c \
../Src/main.c \
../Src/stm32l4xx_hal_msp.c \
../Src/stm32l4xx_it.c \
../Src/system_stm32l4xx.c 

OBJS += \
./Src/daq.o \
./Src/l3gd20h.o \
./Src/lsm303d.o \
./Src/main.o \
./Src/stm32l4xx_hal_msp.o \
./Src/stm32l4xx_it.o \
./Src/system_stm32l4xx.o 

C_DEPS += \
./Src/daq.d \
./Src/l3gd20h.d \
./Src/lsm303d.d \
./Src/main.d \
./Src/stm32l4xx_hal_msp.d \
./Src/stm32l4xx_it.d \
./Src/system_stm32l4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L432xx -I"C:/Users/Chris/Documents/GitHub/IMU/Inc" -I"C:/Users/Chris/Documents/GitHub/IMU/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/Chris/Documents/GitHub/IMU/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Chris/Documents/GitHub/IMU/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Chris/Documents/GitHub/IMU/Drivers/CMSIS/Include"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


