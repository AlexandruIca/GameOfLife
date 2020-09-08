#include "sdl.hpp"

#include "assert.hpp"
#include "log.hpp"

#include <SDL.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace sdl {

initializer::initializer() noexcept
{
    [[maybe_unused]] auto const sdl_init = SDL_Init(SDL_INIT_VIDEO);
    ASSERT_MSG(sdl_init == 0, SDL_GetError());
}

initializer::~initializer() noexcept
{
    SDL_Quit();
}

auto initializer::instance() noexcept -> initializer&
{
    static initializer inst;
    return inst;
}

auto initializer::initialize() noexcept -> void
{
    auto& inst = instance();
    static_cast<void>(inst);
}

window::window(std::string const& title, int const w, int const h) noexcept
{
    initializer::initialize();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = SDL_CreateWindow(
        title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    ASSERT_MSG(m_window != nullptr, SDL_GetError());

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    ASSERT_MSG(m_renderer != nullptr, SDL_GetError());

    m_gl_context = SDL_GL_CreateContext(m_window);

    [[maybe_unused]] auto const glad_loaded = gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress));
    ASSERT(glad_loaded != 0);

    SDL_GL_SetSwapInterval(1);

    INFO("OpenGL context created! Version {}.{}", GLVersion.major, GLVersion.minor);
}

window::~window() noexcept
{
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
}

auto window::should_close() const noexcept -> bool
{
    return m_should_close;
}

auto window::request_close() noexcept -> void
{
    m_should_close = true;
}

auto window::swap_buffers() const noexcept -> void
{
    SDL_GL_SwapWindow(m_window);
}

auto window::get_mouse_coord() const noexcept -> mouse_coord_t
{
    static_cast<void>(m_window); // ignore 'method can be made static'
    int x = 0;
    int y = 0;

    SDL_GetMouseState(&x, &y);

    return { x, y };
}

auto window::width() const noexcept -> int
{
    return m_width;
}

auto window::height() const noexcept -> int
{
    return m_height;
}

auto window::handle_events() -> void
{
    SDL_Event ev;

    while(static_cast<bool>(SDL_PollEvent(&ev))) {
        switch(ev.type) {
        case SDL_QUIT: {
            m_should_close = true;
            break;
        }
        case SDL_WINDOWEVENT: {
            if(ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_width = ev.window.data1;
                m_height = ev.window.data2;
                glViewport(0, 0, m_width, m_height);
                m_on_resize(m_width, m_height);
            }
            break;
        }
        case SDL_KEYDOWN: {
            key_event key = key_event::vk_none;

            switch(ev.key.keysym.sym) {
            case SDLK_ESCAPE: {
                key = key_event::vk_escape;
                break;
            }
            case SDLK_SPACE: {
                key = key_event::vk_space;
                break;
            }
            case SDLK_UP: {
                key = key_event::vk_up;
                break;
            }
            case SDLK_DOWN: {
                key = key_event::vk_down;
                break;
            }
            case SDLK_LEFT: {
                key = key_event::vk_left;
                break;
            }
            case SDLK_RIGHT: {
                key = key_event::vk_right;
                break;
            }
            case SDLK_w: {
                key = key_event::vk_w;
                break;
            }
            case SDLK_s: {
                key = key_event::vk_s;
                break;
            }
            }

            m_on_key_press(key);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if(ev.button.button == SDL_BUTTON_LEFT) {
                m_on_left_click({ ev.button.x, ev.button.y });
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            if(ev.button.button == SDL_BUTTON_LEFT) {
                m_on_left_click_up({ ev.button.x, ev.button.y });
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            m_on_scroll({ ev.wheel.x, ev.wheel.y });
            break;
        }
        default: {
            break;
        }
        }
    }
}

} // namespace sdl
