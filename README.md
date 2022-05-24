Following tools are needed to build the project:
* **GNU Arm Embedded Toolchain.**
* **CMake.**
* **Ninja.**

To build the project use following commands*:

>rm -r build/\* <br><br>
>cmake -DBUILD_TARGET=**target** -B build -G "Ninja" <br><br>
>cmake --build build -t all

\*replace **target** with *arm* or *x86*
