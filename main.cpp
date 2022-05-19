#include "bsp.hpp"

#include <cstdio>

using namespace std;

int main()
{
    using namespace ::smartwatch;
    using namespace std::literals::chrono_literals;
    bsp::init();

    while(true)
    {
        bsp::toggle_led();
        bsp::delay(100ms);
    }

}
