#include "log.hpp"
#include "preview_scene.hpp"
#include "sdl.hpp"
#include "view.hpp"

#include <algorithm>
#include <chrono>
#include <string>

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

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    constexpr int num_cells = 50;

    sdl::window window{ "GameOfLife" };
    gol::view view{ num_cells, num_cells };

    gol::coord const pos = { 25, 25 };
    view.set_alive(pos);

    gol::coord const pos2 = { 24, 24 };
    view.set_alive(pos2);

    gol::coord const pos3 = { 49, 49 };
    view.set_alive(pos3);

    using namespace std::chrono;
    float elapsed = 0.0F;

    gol::preview_scene scene;

    scene.setup_event_handling(window, view);

    bool dragging = false;
    bool toggle = false;
    gol::coord last_coord = { -1, -1 };

    window.on_left_click([&dragging](sdl::mouse_coord_t const c) noexcept -> void {
        TRACE("Left click at (x={}, y={})", c.first, c.second);
        dragging = true;
    });

    window.on_left_click_up([&dragging, &last_coord](sdl::mouse_coord_t const c) noexcept -> void {
        dragging = false;
        last_coord = { -1, -1 };
        TRACE("Left click released at (x={}, y={})", c.first, c.second);
    });

    window.on_scroll([&view, &elapsed](sdl::mouse_coord_t const c) noexcept -> void {
        if(c.second != 0) {
            TRACE("Scroll up/down");

            float new_fov = view.get_fov() - static_cast<float>(c.second) * elapsed;

            TRACE("New fov: {}", new_fov);

            constexpr float fov_max = 47.1F;
            constexpr float fov_min = 44.0F;

            new_fov = std::clamp(new_fov, fov_min, fov_max);
            view.set_fov(new_fov);
        }
    });

    window.on_resize([&view](int const w, int const h) noexcept -> void {
        TRACE("Window resized: w={}, h={}", w, h);
        view.set_aspect_ratio(static_cast<float>(w) / static_cast<float>(h));
    });

    auto start = steady_clock::now();
    constexpr float to_seconds = 1000.0F;

    while(!window.should_close()) {
        auto end = steady_clock::now();
        elapsed = static_cast<float>(duration_cast<milliseconds>(end - start).count()) / to_seconds;
        start = end;

        if(dragging) {
            auto const [grid_x, grid_y] = screen_to_grid(window, view, window.get_mouse_coord());

            if(gol::coord{ grid_x, grid_y } != last_coord) {
                toggle = true;
                last_coord = { grid_x, grid_y };
            }
            else {
                toggle = false;
            }

            if(toggle && grid_x >= 0 && grid_x < view.width() && grid_y >= 0 && grid_y < view.height()) {
                view.toggle_at({ grid_x, grid_y });
            }
        }

        window.handle_events();
        scene.update(elapsed);
        view.update();
        window.swap_buffers();
    }
}
