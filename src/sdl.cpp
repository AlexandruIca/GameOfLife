#include "sdl.hpp"

#include <SDL.h>

namespace sdl {

initializer::initializer() noexcept
{
    SDL_Init(SDL_INIT_VIDEO);
}

initializer::~initializer() noexcept
{
    SDL_Quit();
}

auto initializer::instance() noexcept -> initializer&
{
    static initializer inst;
    return inst;
}

auto initializer::initialize() noexcept -> void
{
    auto& inst = instance();
    static_cast<void>(inst);
}

} // namespace sdl
