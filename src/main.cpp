#include "log.hpp"
#include "sdl.hpp"
#include "view.hpp"

#include <chrono>
#include <string>

auto key_to_string(sdl::key_event const key) -> std::string
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

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    constexpr int num_cells = 50;
    constexpr float translate_offset = 10.0F;

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

    window.on_key_press([&window, &view, &elapsed](sdl::key_event const ev) noexcept -> void {
        TRACE("{} pressed!", key_to_string(ev));
        switch(ev) {
        case sdl::key_event::vk_escape: {
            window.request_close();
            break;
        }
        case sdl::key_event::vk_up: {
            view.translate({ 0.0F, 0.0F, translate_offset * elapsed });
            break;
        }
        case sdl::key_event::vk_down: {
            view.translate({ 0.0F, 0.0F, -translate_offset * elapsed });
            break;
        }
        case sdl::key_event::vk_left: {
            view.translate({ translate_offset * elapsed, 0.0F, 0.0F });
            break;
        }
        case sdl::key_event::vk_right: {
            view.translate({ -translate_offset * elapsed, 0.0F, 0.0F });
            break;
        }
        case sdl::key_event::vk_w: {
            view.translate({ 0.0F, -translate_offset * elapsed, 0.0F });
            break;
        }
        case sdl::key_event::vk_s: {
            view.translate({ 0.0F, translate_offset * elapsed, 0.0F });
            break;
        }
        default: {
            break;
        }
        }
    });

    window.on_left_click(
        [](sdl::mouse_coord_t const c) noexcept -> void { TRACE("Left click at (x={}, y={})", c.first, c.second); });

    window.on_left_click_up([](sdl::mouse_coord_t const c) noexcept -> void {
        TRACE("Left click released at (x={}, y={})", c.first, c.second);
    });

    window.on_scroll([](sdl::mouse_coord_t const c) noexcept -> void {
        if(c.second != 0) {
            TRACE("Scroll up/down");
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

        window.handle_events();
        view.update();
        window.swap_buffers();
    }
}
