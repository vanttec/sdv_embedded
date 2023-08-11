################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/stepper_lib/encoder.c \
../libs/stepper_lib/stepper.c \
../libs/stepper_lib/stepper_tasks.c 

C_DEPS += \
./libs/stepper_lib/encoder.d \
./libs/stepper_lib/stepper.d \
./libs/stepper_lib/stepper_tasks.d 

OBJS += \
./libs/stepper_lib/encoder.o \
./libs/stepper_lib/stepper.o \
./libs/stepper_lib/stepper_tasks.o 


# Each subdirectory must supply rules for building sources it contributes
libs/stepper_lib/%.o libs/stepper_lib/%.su libs/stepper_lib/%.cyclo: ../libs/stepper_lib/%.c libs/stepper_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libs/vanttec_CANLib/src/Vanttec_CANLib -I../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm -I../libs/stepper_lib -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-stepper_lib

clean-libs-2f-stepper_lib:
	-$(RM) ./libs/stepper_lib/encoder.cyclo ./libs/stepper_lib/encoder.d ./libs/stepper_lib/encoder.o ./libs/stepper_lib/encoder.su ./libs/stepper_lib/stepper.cyclo ./libs/stepper_lib/stepper.d ./libs/stepper_lib/stepper.o ./libs/stepper_lib/stepper.su ./libs/stepper_lib/stepper_tasks.cyclo ./libs/stepper_lib/stepper_tasks.d ./libs/stepper_lib/stepper_tasks.o ./libs/stepper_lib/stepper_tasks.su

.PHONY: clean-libs-2f-stepper_lib

