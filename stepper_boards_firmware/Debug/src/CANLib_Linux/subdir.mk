################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/saveasmtz/Documents/embedded/vanttec_CANLib/src/CANLib_Linux/CANHandler.cpp 

OBJS += \
./src/CANLib_Linux/CANHandler.o 

CPP_DEPS += \
./src/CANLib_Linux/CANHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/CANLib_Linux/CANHandler.o: /home/saveasmtz/Documents/embedded/vanttec_CANLib/src/CANLib_Linux/CANHandler.cpp src/CANLib_Linux/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"/home/saveasmtz/Documents/embedded/vanttec_CANLib/src" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-CANLib_Linux

clean-src-2f-CANLib_Linux:
	-$(RM) ./src/CANLib_Linux/CANHandler.d ./src/CANLib_Linux/CANHandler.o ./src/CANLib_Linux/CANHandler.su

.PHONY: clean-src-2f-CANLib_Linux

