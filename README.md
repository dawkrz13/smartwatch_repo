Following tools are needed to build the app:
* GNU Arm Embedded Toolchain
* CMake
* Ninja

Follow these steps:

* Generate STM32CubeMX project code.
Project path: `bsp\nucleo-l476rg\CubeMX\nucleo-l476rg.ioc`
* In `stm32f*xx_it.c` (interrupt service routines file), add:
```c
#include "max30100_for_stm32_hal.h"
```
* In the corresponding interrupt handler function - `EXTI2_IRQHandler()` - call the interrupt handler:
```c
MAX30100_InterruptHandler();
```
* To build the project use following commands*:
```
cmake -DBUILD_TARGET=target -B build -G "Ninja"
```
```
cmake --build build -t all
```
\*replace **target** with *arm* or *x86*

**MAX30100 library repo:**
[MAX30100 library](https://github.com/dawkrz13/MAX30100_for_STM32_HAL)
