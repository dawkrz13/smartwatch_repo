#pragma once

#include <cstdint>
#include <span>

namespace smartwatch::bsp
{
    std::span<char const> write(std::span<char const> data);

    std::span<char> read(std::span<char> buffer);
}
