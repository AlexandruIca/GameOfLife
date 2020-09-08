#ifndef GOL_GOL_SCENE_HPP
#define GOL_GOL_SCENE_HPP
#pragma once

#include "coord.hpp"
#include "scene.hpp"

#include <utility>
#include <vector>

namespace gol {

class gol_scene : public scene
{
private:
    static constexpr int s_translate_offset = 10.0F;
    static constexpr unsigned char s_alive = 1;
    static constexpr unsigned char s_dead = 0;

    std::vector<unsigned char> m_grid;
    // m_events[i].second == true <=> set_alive
    std::vector<std::pair<coord, bool>> m_events;
    int m_width = 0;
    int m_height = 0;
    sdl::window* m_window = nullptr;
    gol::view* m_view = nullptr;
    float m_elapsed = 0.0F;
    gol::coord m_last_mouse_coord = { 0, 0 };
    bool m_dragging = false;
    bool m_finished = false;

    auto initialize_grid(std::vector<coord> const& initial_alive_cells) noexcept -> void;
    [[nodiscard]] auto cell_at(coord pos) noexcept -> unsigned char&;
    [[nodiscard]] auto cell_at(coord pos) const noexcept -> unsigned char const&;
    [[nodiscard]] auto count_at(coord pos) const noexcept -> int;

public:
    gol_scene() noexcept = default;
    gol_scene(gol_scene const&) noexcept = default;
    gol_scene(gol_scene&&) noexcept = default;
    ~gol_scene() noexcept override = default;

    auto operator=(gol_scene const&) noexcept -> gol_scene& = default;
    auto operator=(gol_scene&&) noexcept -> gol_scene& = default;

    auto setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void override;
    auto update(float elapsed) noexcept -> void override;
    [[nodiscard]] auto finished() const noexcept -> bool override;
};

} // namespace gol

#endif // !GOL_PREVIEW_SCENE_HPP
