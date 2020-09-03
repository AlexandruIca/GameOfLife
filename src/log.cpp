#include "log.hpp"

#include <array>

namespace logging {

logger::logger()
{
    std::array<spdlog::sink_ptr, 2> sinks = { std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
                                              std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                                                  "GameOfLife.txt", /* truncate: */ true) };

    m_logger = std::make_shared<spdlog::logger>("GameOfLife", sinks.begin(), sinks.end());
    spdlog::register_logger(m_logger);
    m_logger->set_level(spdlog::level::trace);
    m_logger->flush_on(spdlog::level::trace);
}

auto logger::get() noexcept -> std::shared_ptr<spdlog::logger>&
{
    static logger inst;
    return inst.m_logger;
}

} // namespace logging
