add_library(max30100
    src/max30100.cpp
)

target_include_directories(max30100 PUBLIC include)

target_link_libraries(max30100
    PUBLIC
        sensor_interface
    PRIVATE
        stm32_hal
        bsp
)
