#include "view.hpp"

#include "assert.hpp"
#include "log.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <cstdlib>
#include <string>

namespace {

struct program_description
{
    std::string vertex_shader_source;
    std::string fragment_shader_source;
};

[[nodiscard]] auto create_shader(std::string const& source, unsigned int const type) -> unsigned int
{
    unsigned int shader = glCreateShader(type);

    char const* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int succeded = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &succeded);

    if(succeded != GL_TRUE) {
        int len = 0;
        std::string msg;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        msg.resize(static_cast<std::size_t>(len));
        glGetShaderInfoLog(shader, static_cast<int>(msg.size()), nullptr, msg.data());

        std::string const shader_type = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        FATAL("Compilation of {} shader failed: {}", shader_type, msg);
    }

    return shader;
}

[[nodiscard]] auto create_program(program_description const& desc) -> unsigned int
{
    unsigned int program = glCreateProgram();

    auto const vs = create_shader(desc.vertex_shader_source, GL_VERTEX_SHADER);
    auto const fs = create_shader(desc.fragment_shader_source, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int succeded = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &succeded);

    if(succeded != GL_TRUE) {
        int len = 0;
        std::string msg;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        msg.resize(static_cast<std::size_t>(len));
        glGetProgramInfoLog(program, static_cast<int>(msg.size()), nullptr, msg.data());

        FATAL("Program could not be linked: {}", msg);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

auto set_mat4(unsigned int program, std::string const& var, glm::mat4 const& mat) noexcept -> void
{
    glUniformMatrix4fv(glGetUniformLocation(program, var.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

auto update_cam(unsigned int program, std::string const& var, gol::camera const& cam) noexcept -> void
{
    glUseProgram(program);
    set_mat4(program, var, cam.view());
    glUseProgram(0);
}

} // namespace

namespace gol {

camera::camera(glm::vec3 const& pos, glm::quat const& orient) noexcept
    : m_pos{ pos }
    , m_orient{ orient }
    , m_translation{ 0, 0, 0 }
{
}
camera::camera(glm::vec3 const& pos) noexcept
    : camera(pos, glm::quat{})
{
}

auto camera::position() const noexcept -> glm::vec3 const&
{
    return m_pos;
}

auto camera::orientation() const noexcept -> glm::quat const&
{
    return m_orient;
}

auto camera::view() const noexcept -> glm::mat4
{
    return glm::translate(glm::mat4_cast(m_orient), m_pos);
}

auto camera::translate(glm::vec3 const& v) noexcept -> void
{
    m_pos += v * m_orient;
    m_translation += v;
}

auto camera::translate(float const x, float const y, float const z)
{
    this->translate(glm::vec3{ x, y, z });
}

auto camera::rotate(float const angle, glm::vec3 const& axis) noexcept -> void
{
    m_orient *= glm::angleAxis(angle, axis * m_orient);
}

auto camera::rotate(float const angle, float const x, float const y, float const z) noexcept -> void
{
    this->rotate(angle, glm::vec3{ x, y, z });
}

auto camera::yaw(float const angle) noexcept -> void
{
    this->rotate(angle, 0.0F, 1.0F, 0.0F);
}

auto camera::pitch(float const angle) noexcept -> void
{
    this->rotate(angle, 1.0F, 0.0F, 0.0F);
}

auto camera::roll(float const angle) noexcept -> void
{
    this->rotate(angle, 0.0F, 0.0F, 1.0F);
}

auto camera::translation() const noexcept -> glm::vec3 const&
{
    return m_translation;
}

span::span(vertex& v) noexcept
    : m_ptr{ &v }
{
}

auto span::operator[](std::size_t const index) noexcept -> vertex&
{
    ASSERT(index < m_len);
    return m_ptr[index]; // NOLINT
}

auto span::operator[](std::size_t const index) const noexcept -> vertex const&
{
    ASSERT(index < m_len);
    return m_ptr[index]; // NOLINT
}

auto span::operator[](int const index) noexcept -> vertex&
{
    return this->operator[](static_cast<std::size_t>(index));
}

auto span::operator[](int const index) const noexcept -> vertex const&
{
    return this->operator[](static_cast<std::size_t>(index));
}

auto span::ptr() noexcept -> vertex*
{
    return m_ptr;
}

view::view(int const w, int const h, color const& a, color const& d)
    : m_cell_color{ a }
    , m_dead_cell_color{ d }
    , m_width{ w }
    , m_height{ h }
{
    ASSERT(w > 0);
    ASSERT(h > 0);

    constexpr int vertices_per_cell = s_vertices_per_cell;
    constexpr int indices_per_cell = 6;

    m_cells.resize(static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * vertices_per_cell);
    m_indices.reserve(m_cells.size() * indices_per_cell);

    float const total_width = s_cell_dim * static_cast<float>(w);
    float const total_height = s_cell_dim * static_cast<float>(h);
    float const start_x = -total_width / 2.0F;
    float const start_y = total_height / 2.0F;
    float fy = start_y;

    std::size_t index = 0;
    unsigned int vertex_index = 0;

    for(int y = 0; y < h; ++y) {
        float fx = start_x;
        for(int x = 0; x < w; ++x) {
            constexpr float offset = s_cell_offset;
            auto cell = span{ m_cells[index] };

            // top left
            cell[0].x = fx + offset;
            cell[0].y = fy + offset;
            // top right
            cell[1].x = fx + s_cell_dim - offset;
            cell[1].y = fy + offset;
            // bottom right
            cell[2].x = fx + s_cell_dim - offset;
            cell[2].y = fy + s_cell_dim - offset;
            // bottom left
            cell[3].x = fx + offset;
            cell[3].y = fy + s_cell_dim - offset;

            index += vertices_per_cell;
            fx += s_cell_dim;

            for(int i = 0; i < s_vertices_per_cell; ++i) {
                cell[i].r = m_dead_cell_color.r;
                cell[i].g = m_dead_cell_color.g;
                cell[i].b = m_dead_cell_color.b;
            }

            m_indices.push_back(vertex_index);
            m_indices.push_back(vertex_index + 3);
            m_indices.push_back(vertex_index + 2);
            m_indices.push_back(vertex_index + 2);
            m_indices.push_back(vertex_index);
            m_indices.push_back(vertex_index + 1);
            vertex_index += vertices_per_cell;
        }

        fy -= s_cell_dim;
    }

    program_description desc;

    desc.vertex_shader_source = R"(
    #version 330 core

    layout(location = 0) in vec2 pos;
    layout(location = 1) in vec3 color;

    uniform mat4 projection;
    uniform mat4 view;

    out vec4 f_color;

    void main() {
        gl_Position = projection * view * vec4(pos, 0.0, 1.0);
        f_color = vec4(color, 1.0);
    }
    )";

    desc.fragment_shader_source = R"(
    #version 330 core

    in vec4 f_color;
    out vec4 frag_color;

    void main() {
        frag_color = f_color;
    }
    )";

    m_program = create_program(desc);

    glUseProgram(m_program);
    set_mat4(m_program, "projection", glm::perspective(m_fov, m_aspect_ratio, m_near, m_far));
    set_mat4(m_program, "view", m_camera.view());
    glUseProgram(0);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_cells.size() * sizeof(vertex)), m_cells.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, r))); // NOLINT
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
                 m_indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

view::~view() noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_program);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
}

auto view::cell_at(coord const pos) noexcept -> span
{
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    return span{ m_cells[static_cast<std::size_t>(pos.y) * static_cast<std::size_t>(m_width) * s_vertices_per_cell +
                         static_cast<std::size_t>(pos.x) * s_vertices_per_cell] };
}

auto view::set_alive_impl(coord const pos) noexcept -> void
{
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    auto cell = this->cell_at(pos);

    for(int i = 0; i < s_vertices_per_cell; ++i) {
        cell[i].r = m_cell_color.r;
        cell[i].g = m_cell_color.g;
        cell[i].b = m_cell_color.b;
    }

    auto const offset =
        static_cast<unsigned long>(pos.y * m_width * s_vertices_per_cell + pos.x * s_vertices_per_cell) *
        sizeof(vertex);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), sizeof(vertex) * s_vertices_per_cell, cell.ptr());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

auto view::set_dead_impl(coord const pos) noexcept -> void
{
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    auto cell = this->cell_at(pos);

    for(int i = 0; i < s_vertices_per_cell; ++i) {
        cell[i].r = m_dead_cell_color.r;
        cell[i].g = m_dead_cell_color.g;
        cell[i].b = m_dead_cell_color.b;
    }

    auto const offset =
        static_cast<unsigned long>(pos.y * m_width * s_vertices_per_cell + pos.x * s_vertices_per_cell) *
        sizeof(vertex);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), sizeof(vertex) * s_vertices_per_cell, cell.ptr());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

auto view::set_alive(coord const pos) noexcept -> void
{
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    m_initial_alive_cells.insert(pos);
    this->set_alive_impl(pos);
}

auto view::set_dead(coord const pos) noexcept -> void
{
    ASSERT(pos.x < m_width);
    ASSERT(pos.y < m_height);

    m_initial_alive_cells.erase(pos);
    this->set_dead_impl(pos);
}

auto view::set_cell_color(float const r, float const g, float const b) noexcept -> void
{
    m_cell_color.r = r;
    m_cell_color.g = g;
    m_cell_color.b = b;
}

auto view::set_dead_cell_color(float const r, float const g, float const b) noexcept -> void
{
    m_dead_cell_color.r = r;
    m_dead_cell_color.g = g;
    m_dead_cell_color.b = b;
}

auto view::update() noexcept -> void
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

auto view::set_aspect_ratio(float const a) noexcept -> void
{
    m_aspect_ratio = a;
    glUseProgram(m_program);
    set_mat4(m_program, "projection", glm::perspective(m_fov, a, m_near, m_far));
    glUseProgram(0);
}

auto view::set_fov(float const fov) noexcept -> void
{
    m_fov = fov;
    glUseProgram(m_program);
    set_mat4(m_program, "projection", glm::perspective(fov, m_aspect_ratio, m_near, m_far));
    glUseProgram(0);
}

auto view::get_fov() const noexcept -> float
{
    return m_fov;
}

auto view::translate(glm::vec3 const v) noexcept -> void
{
    m_camera.translate(v);
    update_cam(m_program, "view", m_camera);
}

auto view::view_matrix() const noexcept -> glm::mat4
{
    return m_camera.view();
}

auto view::projection_matrix() const noexcept -> glm::mat4
{
    return glm::perspective(m_fov, m_aspect_ratio, m_near, m_far);
}

auto view::near() const noexcept -> float
{
    return m_near;
}

auto view::translation() const noexcept -> glm::vec3 const&
{
    return m_camera.translation();
}

auto view::width() const noexcept -> int
{
    return m_width;
}

auto view::height() const noexcept -> int
{
    return m_height;
}

auto view::toggle_at(gol::coord const& pos) noexcept -> void
{
    if(m_initial_alive_cells.find(pos) != m_initial_alive_cells.end()) {
        this->set_dead(pos);
    }
    else {
        this->set_alive(pos);
    }
}

auto view::get_initial_alive_cells() const noexcept -> std::set<coord> const&
{
    return m_initial_alive_cells;
}

} // namespace gol
