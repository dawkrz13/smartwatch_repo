#include "bsp.hpp"

#include <cstdio>

using namespace std;

int main()
{
    using namespace ::smartwatch;
    using namespace std::literals::chrono_literals;
    bsp::init();

    bsp::write("Smartwatch App v1.0\r\n");
    while(true)
    {
        char c;
        auto user_command = bsp::read({&c, 1});
        if(!user_command.empty())
        {
            bsp::write(user_command);
            bsp::write("\r\n");
        }
        bsp::toggle_led();
        bsp::delay(500ms);
    }

}
