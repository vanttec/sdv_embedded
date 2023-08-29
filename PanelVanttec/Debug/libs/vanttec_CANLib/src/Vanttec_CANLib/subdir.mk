################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.cpp 

OBJS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.o 

CPP_DEPS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.d 


# Each subdirectory must supply rules for building sources it contributes
libs/vanttec_CANLib/src/Vanttec_CANLib/%.o libs/vanttec_CANLib/src/Vanttec_CANLib/%.su libs/vanttec_CANLib/src/Vanttec_CANLib/%.cyclo: ../libs/vanttec_CANLib/src/Vanttec_CANLib/%.cpp libs/vanttec_CANLib/src/Vanttec_CANLib/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libs/vanttec_CANLib/src/Vanttec_CANLib -I../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib

clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib:
	-$(RM) ./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.cyclo ./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.d ./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.o ./libs/vanttec_CANLib/src/Vanttec_CANLib/CANMessage.su

.PHONY: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib

