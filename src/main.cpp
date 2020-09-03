#include "log.hpp"
#include "sdl.hpp"

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
    sdl::window window{ "GameOfLife" };

    window.on_key_press([&window](sdl::key_event const ev) noexcept -> void {
        TRACE("{} pressed!", key_to_string(ev));
        if(ev == sdl::key_event::vk_escape) {
            window.request_close();
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

    window.on_resize([](int const w, int const h) noexcept -> void { TRACE("Window resized: w={}, h={}", w, h); });

    while(!window.should_close()) {
        window.handle_events();
        window.swap_buffers();
    }
}
