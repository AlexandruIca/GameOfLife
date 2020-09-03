#ifndef GOL_LOG_HPP
#define GOL_LOG_HPP
#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace logging {

struct logger
{
private:
    std::shared_ptr<spdlog::logger> m_logger;

    logger();

public:
    logger(logger const&) = delete;
    logger(logger&&) noexcept = delete;
    ~logger() noexcept = default;

    auto operator=(logger const&) -> logger& = delete;
    auto operator=(logger&&) noexcept -> logger& = delete;

    [[nodiscard]] static auto get() noexcept -> std::shared_ptr<spdlog::logger>&;
};

} // namespace logging

#ifdef GOL_DEBUG

#define TRACE(...) ::logging::logger::get()->trace(__VA_ARGS__)
#define INFO(...) ::logging::logger::get()->info(__VA_ARGS__)
#define WARN(...) ::logging::logger::get()->warn(__VA_ARGS__)
#define ERROR(...) ::logging::logger::get()->error(__VA_ARGS__)
#define FATAL(...) ::logging::logger::get()->critical(__VA_ARGS__)

#else

#define TRACE(...) static_cast<void>(0)
#define INFO(...) static_cast<void>(0)
#define WARN(...) static_cast<void>(0)
#define ERROR(...) static_cast<void>(0)
#define FATAL(...) static_cast<void>(0)

#endif

#endif // !GOL_LOG_HPP
