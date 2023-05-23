################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.c 

C_DEPS += \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.d 

OBJS += \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.o 


# Each subdirectory must supply rules for building sources it contributes
libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/%.o libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/%.su: ../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/%.c libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"../libs/vanttec_CANLib/src/Vanttec_CANLib" -I"../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm

clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm:
	-$(RM) ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/ping_task.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/utils.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_rx_task.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/vanttec_canlib_tx_task.su

.PHONY: clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm

