################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.c \
../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.c 

C_DEPS += \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.d \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.d 

OBJS += \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.o \
./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.o 


# Each subdirectory must supply rules for building sources it contributes
libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/%.o libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/%.su: ../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/%.c libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libs/vanttec_CANLib/src/Vanttec_CANLib -I../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm -I../libs/STEPPER -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm-2f-generic_tasks

clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm-2f-generic_tasks:
	-$(RM) ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/can_reset_task.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/generic_tasks.su ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.d ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.o ./libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm/generic_tasks/hb_task.su

.PHONY: clean-libs-2f-vanttec_CANLib-2f-platform_specific-2f-vanttec_canlib_stm-2f-generic_tasks

