################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/CubeIDE/h503_Git/m100-m3-mini-board-stm32f103/libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.c 

OBJS += \
./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.o 

C_DEPS += \
./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.o: F:/CubeIDE/h503_Git/m100-m3-mini-board-stm32f103/libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.c libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../BSP/LED -I../BSP/KEY -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../../libraries/Drivers/STM32F1xx_HAL_Driver/Inc -I../../../libraries/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../../libraries/Drivers/CMSIS/Include -I../BSP/LCD -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-libraries-2f-Drivers-2f-STM32F1xx_HAL_Driver-2f-Src-2f-Legacy

clean-libraries-2f-Drivers-2f-STM32F1xx_HAL_Driver-2f-Src-2f-Legacy:
	-$(RM) ./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.cyclo ./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.d ./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.o ./libraries/Drivers/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.su

.PHONY: clean-libraries-2f-Drivers-2f-STM32F1xx_HAL_Driver-2f-Src-2f-Legacy
