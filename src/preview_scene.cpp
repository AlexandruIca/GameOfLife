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

} // namespace

namespace gol {

auto preview_scene::setup_event_handling(sdl::window& window, gol::view& view) noexcept -> void
{
    window.on_key_press([this, &window, &view](sdl::key_event const ev) noexcept -> void {
        TRACE("[Scene] {} pressed!", key_to_string(ev));
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
}

auto preview_scene::update(float const elapsed) noexcept -> void
{
    m_elapsed = elapsed;
}

} // namespace gol
