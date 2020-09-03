#ifndef GOL_SDL_HPP
#define GOL_SDL_HPP
#pragma once

namespace sdl {

struct initializer
{
private:
    initializer() noexcept;

    [[nodiscard]] static auto instance() noexcept -> initializer&;

public:
    initializer(initializer const&) = delete;
    initializer(initializer&&) noexcept = delete;
    ~initializer() noexcept;

    auto operator=(initializer const&) -> initializer& = delete;
    auto operator=(initializer&&) noexcept -> initializer& = delete;

    static auto initialize() noexcept -> void;
};

} // namespace sdl

#endif // !GOL_SDL_HPP
