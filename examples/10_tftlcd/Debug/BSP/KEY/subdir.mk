################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/KEY/key.c 

OBJS += \
./BSP/KEY/key.o 

C_DEPS += \
./BSP/KEY/key.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/KEY/%.o BSP/KEY/%.su BSP/KEY/%.cyclo: ../BSP/KEY/%.c BSP/KEY/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../BSP/LED -I../BSP/KEY -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc -I../../../libraries/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../../libraries/Drivers/CMSIS/Include -I../BSP/LCD -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BSP-2f-KEY

clean-BSP-2f-KEY:
	-$(RM) ./BSP/KEY/key.cyclo ./BSP/KEY/key.d ./BSP/KEY/key.o ./BSP/KEY/key.su

.PHONY: clean-BSP-2f-KEY

