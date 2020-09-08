#include "gol_scene.hpp"
#include "log.hpp"
#include "preview_scene.hpp"
#include "sdl.hpp"
#include "view.hpp"

#include <docopt/docopt.h>

#include <array>
#include <chrono>
#include <map>
#include <memory>
#include <queue>
#include <string>

std::map<std::string, gol::color> const g_colors = {
    { "white", { 1.0F, 1.0F, 1.0F } }, { "black", { 0.0F, 0.0F, 0.0F } },   { "red", { 1.0F, 0.0F, 0.0F } },
    { "blue", { 0.0F, 0.0F, 1.0F } },  { "yellow", { 1.0F, 0.96F, 0.0F } }, { "green", { 0.0F, 1.0F, 0.0F } }
};

std::string const g_usage = R"(GameOfLife

Usage:
    GameOfLife [-h | --help]
                    [(--width=<grid_width> --height=<grid_height>)]
                    [--color-dead=<color_dead>]
                    [--color-alive=<color_alive>]

Options:
    -h --help                       Show this screen.
    --width=<grid_width>            How many cells horizontally [default: 50].
    --height=<grid_height>          How many cells vertically [default: 50].
    --color-dead=<color_dead>       What color should a dead cell have [default: black].
    --color-alive=<color_alive>     What color should an alive cell have [default: white].
)";

auto configure(std::map<std::string, docopt::value>& args, int& num_cells_w, int& num_cells_h) -> void
{
    if(args["--width"].isString() && args["--height"].isString()) {
        num_cells_w = std::stoi(args["--width"].asString());
        num_cells_h = std::stoi(args["--height"].asString());
    }
}

auto configure_view(std::map<std::string, docopt::value>& args, gol::color& alive, gol::color& dead) -> void
{
    auto const& white = g_colors.at("white");
    auto const& black = g_colors.at("black");

    alive = white;
    dead = black;

    if(args["--color-dead"].isString()) {
        dead = g_colors.at(args["--color-dead"].asString());
    }
    if(args["--color-alive"].isString()) {
        alive = g_colors.at(args["--color-alive"].asString());
    }
}

auto main(int argc, char* argv[]) noexcept -> int
{
    [[maybe_unused]] auto args = docopt::docopt(g_usage, { argv + 1, argv + argc }, /*show help:*/ true, "GameOfLife");

    constexpr int default_num_cells = 50;
    int num_cells_w = default_num_cells;
    int num_cells_h = default_num_cells;

    configure(args, num_cells_w, num_cells_h);

    gol::color alive_color;
    gol::color dead_color;

    configure_view(args, alive_color, dead_color);

    sdl::window window{ "GameOfLife" };
    gol::view view{ num_cells_w, num_cells_h, alive_color, dead_color };

    gol::coord const pos = { num_cells_w / 2, num_cells_h / 2 };
    view.set_alive(pos);

    std::queue<std::unique_ptr<gol::scene>> scene;

    scene.push(std::make_unique<gol::preview_scene>());
    scene.push(std::make_unique<gol::gol_scene>());

    scene.front()->setup_event_handling(window, view);

    using namespace std::chrono;
    float elapsed = 0.0F;
    auto start = steady_clock::now();
    constexpr float to_seconds = 1000.0F;

    while(!window.should_close()) {
        auto end = steady_clock::now();
        elapsed = static_cast<float>(duration_cast<milliseconds>(end - start).count()) / to_seconds;
        start = end;

        window.handle_events();
        scene.front()->update(elapsed);
        view.update();
        window.swap_buffers();

        if(scene.front()->finished()) {
            scene.pop();

            if(scene.empty()) {
                window.request_close();
                break;
            }
            scene.front()->setup_event_handling(window, view);
        }
    }
}
