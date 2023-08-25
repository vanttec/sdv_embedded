################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.cpp \
../libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.cpp 

OBJS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.o \
./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.o 

CPP_DEPS += \
./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.d \
./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.d 


# Each subdirectory must supply rules for building sources it contributes
libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/%.o libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/%.su libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/%.cyclo: ../libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/%.cpp libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libs/vanttec_CANLib/src/Vanttec_CANLib -I../libs/vanttec_CANLib/platform_specific/vanttec_canlib_stm -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-Utils

clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-Utils:
	-$(RM) ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.cyclo ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.d ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.o ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANDeserialization.su ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.cyclo ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.d ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.o ./libs/vanttec_CANLib/src/Vanttec_CANLib/Utils/CANSerialization.su

.PHONY: clean-libs-2f-vanttec_CANLib-2f-src-2f-Vanttec_CANLib-2f-Utils

