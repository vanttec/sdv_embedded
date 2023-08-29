################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.cpp 

OBJS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.o 

CPP_DEPS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.d 


# Each subdirectory must supply rules for building sources it contributes
libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/%.o libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/%.su libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/%.cyclo: ../libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/%.cpp libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libs/vanttec_CANLib/src/Vanttec_CANLib -I../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm -I../libs/STEPPER -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-ByteOrder

clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-ByteOrder:
	-$(RM) ./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.cyclo ./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.d ./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.o ./libs/vanttec_CANLib/src/Vanttec_CANLib/ByteOrder/inet.su

.PHONY: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-ByteOrder

