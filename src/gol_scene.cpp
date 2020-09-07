#include "gol_scene.hpp"

#include "assert.hpp"

#include <algorithm>
#include <numeric>

namespace gol {

auto gol_scene::setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void
{
    m_window = &window;
    m_view = &view;

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
