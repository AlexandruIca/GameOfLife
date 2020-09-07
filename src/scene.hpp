#ifndef GOL_SCENE_HPP
#define GOL_SCENE_HPP
#pragma once

#include "sdl.hpp"
#include "view.hpp"

namespace gol {

class scene
{
public:
    scene() noexcept = default;
    scene(scene const&) noexcept = default;
    scene(scene&&) noexcept = default;
    virtual ~scene() noexcept = default;

    auto operator=(scene const&) noexcept -> scene& = default;
    auto operator=(scene&&) noexcept -> scene& = default;

    virtual auto setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void = 0;
    virtual auto update(float elapsed) noexcept -> void = 0;
    virtual auto finished() const noexcept -> bool = 0;
};

} // namespace gol

#endif // !GOL_SCENE_HPP
