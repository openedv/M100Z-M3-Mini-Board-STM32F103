################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/LED/led.c 

OBJS += \
./BSP/LED/led.o 

C_DEPS += \
./BSP/LED/led.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/LED/%.o BSP/LED/%.su BSP/LED/%.cyclo: ../BSP/LED/%.c BSP/LED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../BSP/LED -I../BSP/KEY -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc -I../../../libraries/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../../libraries/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BSP-2f-LED

clean-BSP-2f-LED:
	-$(RM) ./BSP/LED/led.cyclo ./BSP/LED/led.d ./BSP/LED/led.o ./BSP/LED/led.su

.PHONY: clean-BSP-2f-LED

