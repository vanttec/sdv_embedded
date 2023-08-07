################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Drivers/Startup/startup_stm32l431rctx.s 

S_DEPS += \
./Drivers/Startup/startup_stm32l431rctx.d 

OBJS += \
./Drivers/Startup/startup_stm32l431rctx.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Startup/%.o: ../Drivers/Startup/%.s Drivers/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Drivers-2f-Startup

clean-Drivers-2f-Startup:
	-$(RM) ./Drivers/Startup/startup_stm32l431rctx.d ./Drivers/Startup/startup_stm32l431rctx.o

.PHONY: clean-Drivers-2f-Startup

