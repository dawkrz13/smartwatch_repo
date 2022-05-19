add_library(bsp
    src/bsp.cpp
    src/bsp_led.cpp
    src/bsp_clock.cpp
)

add_subdirectory(CubeMX)

target_include_directories(bsp PUBLIC include)

target_include_directories(bsp
    PUBLIC
        Core/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
        Drivers/CMSIS/Include
)

target_link_libraries(bsp
    PUBLIC
        bsp_interface
        stm32_hal
    INTERFACE
        stm32_hal
)