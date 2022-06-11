include(FetchContent)

FetchContent_Declare(max30100_driver_lib
    GIT_REPOSITORY  https://github.com/dawkrz13/MAX30100_for_STM32_HAL.git
)
FetchContent_MakeAvailable(max30100_driver_lib)

add_library(stm32_hal INTERFACE)

target_link_libraries(stm32_hal INTERFACE stm32_hal_obj
    $<TARGET_OBJECTS:stm32_hal_obj>
)

add_library(stm32_hal_obj OBJECT
    Core/Src/main.c
    Core/Src/stm32l4xx_it.c
    Core/Src/stm32l4xx_hal_msp.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
    Core/Src/system_stm32l4xx.c
    startup_stm32l476xx.s
    ${max30100_driver_lib_SOURCE_DIR}/max30100_for_stm32_hal.c
    ${max30100_driver_lib_SOURCE_DIR}/pulse_detector.c
)

target_include_directories(stm32_hal_obj
    PUBLIC
        Core/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
        Drivers/CMSIS/Include
        ${max30100_driver_lib_SOURCE_DIR}
)

target_compile_definitions(stm32_hal_obj PUBLIC USE_HAL_DIRVER STM32L476xx)
