#ifndef GOL_PREVIEW_SCENE_HPP
#define GOL_PREVIEW_SCENE_HPP
#pragma once

#include "coord.hpp"
#include "scene.hpp"

namespace gol {

class preview_scene : public scene
{
private:
    static constexpr int s_translate_offset = 10.0F;

    sdl::window* m_window = nullptr;
    gol::view* m_view = nullptr;
    float m_elapsed = 0.0F;
    bool m_dragging = false;
    bool m_toggle = false;
    bool m_finished = false;
    gol::coord m_last_coord = { -1, -1 };

public:
    preview_scene() noexcept = default;
    preview_scene(preview_scene const&) noexcept = default;
    preview_scene(preview_scene&&) noexcept = default;
    ~preview_scene() noexcept override = default;

    auto operator=(preview_scene const&) noexcept -> preview_scene& = default;
    auto operator=(preview_scene&&) noexcept -> preview_scene& = default;

    auto setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void override;
    auto update(float elapsed) noexcept -> void override;
    [[nodiscard]] auto finished() const noexcept -> bool override;
};

} // namespace gol

#endif // !GOL_PREVIEW_SCENE_HPP
