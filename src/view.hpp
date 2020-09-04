#ifndef GOL_VIEW_HPP
#define GOL_VIEW_HPP
#pragma once

#include <array>
#include <cstddef>
#include <set>
#include <vector>

#include "coord.hpp"

namespace gol {

struct vertex
{
    float x = 0.0F;
    float y = 0.0F;
    float r = 0.0F;
    float g = 0.0F;
    float b = 0.0F;
};

struct color
{
    float r = 0.0F;
    float g = 0.0F;
    float b = 0.0F;
};

class span
{
private:
    vertex* m_ptr = nullptr;
    static constexpr int m_len = 4;

public:
    span() noexcept = default;
    span(span const&) noexcept = default;
    span(span&&) noexcept = default;
    ~span() noexcept = default;

    explicit span(vertex& v) noexcept;

    auto operator=(span const&) noexcept -> span& = default;
    auto operator=(span&&) noexcept -> span& = default;

    [[nodiscard]] auto operator[](std::size_t index) noexcept -> vertex&;
    [[nodiscard]] auto operator[](std::size_t index) const noexcept -> vertex const&;

    [[nodiscard]] auto operator[](int index) noexcept -> vertex&;
    [[nodiscard]] auto operator[](int index) const noexcept -> vertex const&;

    [[nodiscard]] auto ptr() noexcept -> vertex*;
};

class view
{
private:
    std::vector<vertex> m_cells;
    std::set<coord> m_initial_alive_cells;
    std::vector<unsigned int> m_indices;

    static constexpr color s_default_cell_color = { 1.0F, 1.0F, 1.0F };
    static constexpr color s_default_dead_cell_color = { 1.0F, 0.0F, 0.0F };

    color m_cell_color = s_default_cell_color;
    color m_dead_cell_color = s_default_dead_cell_color;

    int m_width = 0;
    int m_height = 0;

    static constexpr float s_cell_dim = 0.1F;
    static constexpr int s_vertices_per_cell = 4;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ibo = 0;
    unsigned int m_program = 0;

    [[nodiscard]] auto cell_at(coord pos) noexcept -> span;

    auto set_alive_impl(coord pos) noexcept -> void;
    auto set_dead_impl(coord pos) noexcept -> void;

public:
    view() = delete;
    view(view const&) = default;
    view(view&&) noexcept = default;
    ~view() noexcept;

    view(int w, int h);

    auto operator=(view const&) -> view& = default;
    auto operator=(view&&) noexcept -> view& = default;

    auto set_alive(coord pos) noexcept -> void;
    auto set_dead(coord pos) noexcept -> void;

    auto set_cell_color(float r, float g, float b) noexcept -> void;
    auto set_dead_cell_color(float r, float g, float b) noexcept -> void;

    auto update() noexcept -> void;
};

} // namespace gol

#endif // !GOL_VIEW_HPP
