#include "log.hpp"
#include "preview_scene.hpp"
#include "sdl.hpp"
#include "view.hpp"

#include <algorithm>
#include <chrono>
#include <string>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    constexpr int num_cells = 50;

    sdl::window window{ "GameOfLife" };
    gol::view view{ num_cells, num_cells };
    gol::preview_scene scene;

    scene.setup_event_handling(window, view);

    float elapsed = 0.0F;

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

    using namespace std::chrono;
    auto start = steady_clock::now();
    constexpr float to_seconds = 1000.0F;

    while(!window.should_close()) {
        auto end = steady_clock::now();
        elapsed = static_cast<float>(duration_cast<milliseconds>(end - start).count()) / to_seconds;
        start = end;

        window.handle_events();
        scene.update(elapsed);
        view.update();
        window.swap_buffers();
    }
}
