#include "gol_scene.hpp"

#include "assert.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>

namespace gol {

auto gol_scene::cell_at(coord const pos) noexcept -> unsigned char&
{
    ASSERT(pos.x >= 0);
    ASSERT(pos.y >= 0);
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    return m_grid[static_cast<std::size_t>(pos.y + 1) * static_cast<std::size_t>(m_width + 1) +
                  static_cast<std::size_t>(pos.x + 1)];
}

auto gol_scene::cell_at(coord const pos) const noexcept -> unsigned char const&
{
    ASSERT(pos.x >= -1);
    ASSERT(pos.y >= -1);
    ASSERT(pos.x <= m_width);
    ASSERT(pos.y <= m_height);

    return m_grid[static_cast<std::size_t>(pos.y + 1) * static_cast<std::size_t>(m_width + 1) +
                  static_cast<std::size_t>(pos.x + 1)];
}

auto gol_scene::initialize_grid(std::vector<coord> const& initial_alive_cells) noexcept -> void
{
    m_grid.resize(static_cast<std::size_t>(m_width + 2) * static_cast<std::size_t>(m_height + 2));
    std::fill(m_grid.begin(), m_grid.end(), s_dead);

    for(coord const& pos : initial_alive_cells) {
        this->cell_at(pos) = s_alive;
    }
}

auto gol_scene::count_at(coord const pos) const noexcept -> int
{
    std::array<coord, 8> const neighbors = { coord{ pos.x - 1, pos.y - 1 }, coord{ pos.x, pos.y - 1 },
                                             coord{ pos.x + 1, pos.y - 1 }, coord{ pos.x - 1, pos.y },
                                             coord{ pos.x + 1, pos.y },     coord{ pos.x - 1, pos.y + 1 },
                                             coord{ pos.x, pos.y + 1 },     coord{ pos.x + 1, pos.y + 1 } };
    int count = 0;

    for(auto const& neighbor : neighbors) {
        count += this->cell_at(neighbor);
    }

    return count;
}

auto gol_scene::setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void
{
    m_width = view.width();
    m_height = view.height();

    auto const& cells = view.get_initial_alive_cells();
    this->initialize_grid(std::vector<coord>{ cells.begin(), cells.end() });

    m_window = &window;
    m_view = &view;

    m_future = m_threadpool.push([] {});

    window.on_key_press([this, &window, &view](sdl::key_event const ev) noexcept -> void {
        switch(ev) {
        case sdl::key_event::vk_escape: {
            window.request_close();
            break;
        }
        case sdl::key_event::vk_up: {
            view.translate({ 0.0F, -s_translate_offset * m_elapsed, 0.0F });
            break;
        }
        case sdl::key_event::vk_down: {
            view.translate({ 0.0F, s_translate_offset * m_elapsed, 0.0F });
            break;
        }
        case sdl::key_event::vk_left: {
            view.translate({ s_translate_offset * m_elapsed, 0.0F, 0.0F });
            break;
        }
        case sdl::key_event::vk_right: {
            view.translate({ -s_translate_offset * m_elapsed, 0.0F, 0.0F });
            break;
        }
        case sdl::key_event::vk_w: {
            view.translate({ 0.0F, 0.0F, s_translate_offset * m_elapsed });
            break;
        }
        case sdl::key_event::vk_s: {
            view.translate({ 0.0F, 0.0F, -s_translate_offset * m_elapsed });
            break;
        }
        default: {
            break;
        }
        }
    });

    window.on_left_click([this](sdl::mouse_coord_t const c) noexcept -> void {
        TRACE("[GOL Scene] Left click at (x={}, y={})", c.first, c.second);
        m_dragging = true;
        m_last_mouse_coord = { c.first, c.second };
    });

    window.on_left_click_up([this]([[maybe_unused]] sdl::mouse_coord_t const c) noexcept -> void {
        TRACE("[GOL Scene] Left click released at (x={}, y={})", c.first, c.second);
        m_dragging = false;
    });

    window.on_scroll([this, &view](sdl::mouse_coord_t const c) noexcept -> void {
        view.translate({ 0.0F, 0.0F, s_translate_offset * static_cast<float>(c.second) * m_elapsed });
    });

    window.on_resize([&view](int const w, int const h) noexcept -> void {
        TRACE("[GOL Scene] Window resized: w={}, h={}", w, h);
        view.set_aspect_ratio(static_cast<float>(w) / static_cast<float>(h));
    });
}

auto gol_scene::update(float const elapsed) noexcept -> void
{
    m_elapsed = elapsed;

    ASSERT(m_window != nullptr);
    ASSERT(m_view != nullptr);

    constexpr int cell_target_die = 2;
    constexpr int cell_target_live = 3;

    m_future.get();

    std::vector<std::pair<coord, bool>> ev;

    if(m_events.pop(ev)) {
        for(auto& event : ev) {
            if(event.second) {
                this->cell_at(event.first) = s_alive;
                m_view->set_alive(event.first);
            }
            else {
                this->cell_at(event.first) = s_dead;
                m_view->set_dead(event.first);
            }
        }
    }

    m_future = m_threadpool.push([this] {
        static std::vector<std::pair<coord, bool>> events;

        for(int i = 0; i < m_height; ++i) {
            for(int j = 0; j < m_width; ++j) {
                auto const count = this->count_at({ j, i });
                bool const alive = this->cell_at({ j, i }) == s_alive;

                if(alive && ((count < cell_target_die) || (count > cell_target_live))) {
                    events.emplace_back(coord{ j, i }, false);
                }
                else if(!alive && count == cell_target_live) {
                    events.emplace_back(coord{ j, i }, true);
                }
            }
        }

        while(!m_events.push(events)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        events.clear();
    });

    if(m_dragging) {
        auto const tmp = m_window->get_mouse_coord();
        gol::coord const mouse_coord = { tmp.first, tmp.second };

        auto z = -m_view->translation().z; // NOLINT
        constexpr float z_min = 0.5F;
        if(z < 0) {
            if(z > -z_min) {
                z = -z_min;
            }
        }
        else {
            if(z < z_min) {
                z = z_min;
            }
        }

        auto const translate_x =
            z * static_cast<float>(mouse_coord.x - m_last_mouse_coord.x) / static_cast<float>(m_window->width());
        auto const translate_y =
            z * -static_cast<float>(mouse_coord.y - m_last_mouse_coord.y) / static_cast<float>(m_window->height());

        m_view->translate(glm::vec3{ translate_x, translate_y, 0.0F });
        m_last_mouse_coord = mouse_coord;
    }
}

auto gol_scene::finished() const noexcept -> bool
{
    return m_finished;
}

} // namespace gol
