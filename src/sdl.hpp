#ifndef GOL_SDL_HPP
#define GOL_SDL_HPP
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>

struct SDL_Window;
struct SDL_Renderer;

using SDL_GLContext = void*;

namespace sdl {

struct initializer
{
private:
    initializer() noexcept;

    [[nodiscard]] static auto instance() noexcept -> initializer&;

public:
    initializer(initializer const&) = delete;
    initializer(initializer&&) noexcept = delete;
    ~initializer() noexcept;

    auto operator=(initializer const&) -> initializer& = delete;
    auto operator=(initializer&&) noexcept -> initializer& = delete;

    static auto initialize() noexcept -> void;
};

enum class key_event
{
    vk_escape,
    vk_space,
    vk_up,
    vk_down,
    vk_left,
    vk_right,
    vk_w,
    vk_s,
    vk_none
};

using mouse_coord_t = std::pair<int, int>;

class window
{
private:
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
    SDL_GLContext m_gl_context{ nullptr };

    bool m_should_close{ false };

    static constexpr int s_default_width = 1280;
    static constexpr int s_default_height = 720;

    int m_width{ s_default_width };
    int m_height{ s_default_height };

    std::function<void(key_event)> m_on_key_press = []([[maybe_unused]] key_event ev) {};
    std::function<void(mouse_coord_t)> m_on_left_click = []([[maybe_unused]] mouse_coord_t ev) {};
    std::function<void(mouse_coord_t)> m_on_left_click_up = []([[maybe_unused]] mouse_coord_t ev) {};
    std::function<void(mouse_coord_t)> m_on_scroll = []([[maybe_unused]] mouse_coord_t ev) {};
    std::function<void(int, int)> m_on_resize = []([[maybe_unused]] int a, [[maybe_unused]] int b) {};

public:
    window() = delete;
    window(window const&) noexcept = default;
    window(window&&) noexcept = default;
    ~window() noexcept;

    explicit window(std::string const& title, int w = s_default_width, int h = s_default_height) noexcept;

    auto operator=(window const&) noexcept -> window& = default;
    auto operator=(window&&) noexcept -> window& = default;

    [[nodiscard]] auto width() const noexcept -> int;
    [[nodiscard]] auto height() const noexcept -> int;

    [[nodiscard]] auto should_close() const noexcept -> bool;
    auto request_close() noexcept -> void;

    auto swap_buffers() const noexcept -> void;

    auto handle_events() -> void;
    [[nodiscard]] auto get_mouse_coord() const noexcept -> mouse_coord_t;

    template<typename F>
    auto on_key_press(F f) -> void
    {
        m_on_key_press = f;
    }

    template<typename F>
    auto on_left_click(F f) -> void
    {
        m_on_left_click = f;
    }

    template<typename F>
    auto on_left_click_up(F f) -> void
    {
        m_on_left_click_up = f;
    }

    template<typename F>
    auto on_scroll(F f) -> void
    {
        m_on_scroll = f;
    }

    template<typename F>
    auto on_resize(F f) -> void
    {
        m_on_resize = f;
    }
};

} // namespace sdl

#endif // !GOL_SDL_HPP
