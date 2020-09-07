#include "gol_scene.hpp"
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
    gol::gol_scene scene;

    gol::coord const pos = { 25, 25 };
    view.set_alive(pos);

    scene.setup_event_handling(window, view);

    using namespace std::chrono;
    float elapsed = 0.0F;
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
