################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.c 

OBJS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.o 

C_DEPS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.o: E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.c SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"E:/HK2_Nam3/TTCS_THoa/SDK_1.0.3_NUCLEO-F401RE-20260119T004621Z-1-003/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

