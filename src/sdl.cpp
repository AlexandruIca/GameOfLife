#include "sdl.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

namespace sdl {

initializer::initializer() noexcept
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::error("Couldn't initialize SDL: {}", SDL_GetError());
    }
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
