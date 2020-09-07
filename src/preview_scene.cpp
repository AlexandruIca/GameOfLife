#include "preview_scene.hpp"

#include "assert.hpp"

#include <string>

namespace {

[[maybe_unused]] auto key_to_string(sdl::key_event const key) -> std::string
{
    switch(key) {
    case sdl::key_event::vk_up:
        return "UP";
    case sdl::key_event::vk_down:
        return "DOWN";
    case sdl::key_event::vk_left:
        return "LEFT";
    case sdl::key_event::vk_right:
        return "RIGHT";
    case sdl::key_event::vk_w:
        return "W";
    case sdl::key_event::vk_s:
        return "S";
    case sdl::key_event::vk_space:
        return "SPACE";
    case sdl::key_event::vk_escape:
        return "ESCAPE";
    default:
        return "unknown";
    }
}

[[nodiscard]] auto screen_to_grid(sdl::window& window, gol::view& view, sdl::mouse_coord_t const c) noexcept
    -> gol::coord
{
    float const x = (2.0F * static_cast<float>(c.first)) / static_cast<float>(window.width()) - 1.0F;
    float const y = -((2.0F * static_cast<float>(c.second)) / static_cast<float>(window.height()) - 1.0F);
    float const z = 1.0F;
    glm::vec3 ray_nds{ x, y, z };
    glm::vec4 const ray_clip = glm::vec4{ ray_nds.x, ray_nds.y, -1.0F, 1.0F }; // NOLINT
    glm::vec4 ray_eye = glm::inverse(view.projection_matrix()) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0F, 0.0F); // NOLINT
    glm::vec3 ray_wor = glm::inverse(view.view_matrix()) * ray_eye;
    // ray_wor = glm::normalize(ray_wor);
    TRACE("wx={}, wy={}", ray_wor.x, ray_wor.y); // NOLINT

    float const grid_width = static_cast<float>(view.width()) * gol::view::cell_dimension();
    float const grid_height = static_cast<float>(view.height()) * gol::view::cell_dimension();

    float const absx = ray_wor.x + grid_width / 2.0F;                                          // NOLINT
    float const absy = std::abs(ray_wor.y - grid_height / 2.0F - gol::view::cell_dimension()); // NOLINT

    auto const grid_x = static_cast<int>(absx / gol::view::cell_dimension());
    auto const grid_y = static_cast<int>(absy / gol::view::cell_dimension());

    return { grid_x, grid_y };
}

} // namespace

namespace gol {

auto preview_scene::setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void
{
    m_window = &window;
    m_view = &view;

    window.on_key_press([this, &window, &view](sdl::key_event const ev) noexcept -> void {
        TRACE("[Preview Scene] {} pressed!", key_to_string(ev));
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
        TRACE("[Preview Scene] Left click at (x={}, y={})", c.first, c.second);
        m_dragging = true;
    });

    window.on_left_click_up([this](sdl::mouse_coord_t const c) noexcept -> void {
        m_dragging = false;
        m_last_coord = { -1, -1 };
        TRACE("[Preview Scene] Left click released at (x={}, y={})", c.first, c.second);
    });

    window.on_scroll([this, &view](sdl::mouse_coord_t const c) noexcept -> void {
        if(c.second != 0) {
            TRACE("[Preview Scene] Scroll up/down");

            float new_fov = view.get_fov() - static_cast<float>(c.second) * m_elapsed;

            TRACE("[Preview Scene] New fov: {}", new_fov);

            constexpr float fov_max = 47.1F;
            constexpr float fov_min = 44.0F;

            new_fov = std::clamp(new_fov, fov_min, fov_max);
            view.set_fov(new_fov);
        }
    });

    window.on_resize([&view](int const w, int const h) noexcept -> void {
        TRACE("[Preview Scene] Window resized: w={}, h={}", w, h);
        view.set_aspect_ratio(static_cast<float>(w) / static_cast<float>(h));
    });
}

auto preview_scene::update(float const elapsed) noexcept -> void
{
    m_elapsed = elapsed;

    ASSERT(m_window != nullptr);
    ASSERT(m_view != nullptr);

    if(m_dragging) {
        auto const [grid_x, grid_y] = screen_to_grid(*m_window, *m_view, m_window->get_mouse_coord());

        if(gol::coord{ grid_x, grid_y } != m_last_coord) {
            m_toggle = true;
            m_last_coord = { grid_x, grid_y };
        }
        else {
            m_toggle = false;
        }

        if(m_toggle && grid_x >= 0 && grid_x < m_view->width() && grid_y >= 0 && grid_y < m_view->height()) {
            m_view->toggle_at({ grid_x, grid_y });
        }
    }
}

} // namespace gol
