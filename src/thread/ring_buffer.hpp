#ifndef GOL_THREAD_RING_BUFFER_HPP
#define GOL_THREAD_RING_BUFFER_HPP
#pragma once

#include <array>
#include <atomic>
#include <utility>

namespace gol {

template<typename T, std::size_t N>
class ring_buffer
{
private:
    std::array<T, N> m_ring;
    std::atomic<std::size_t> m_head;
    std::atomic<std::size_t> m_tail;

    [[nodiscard]] static auto next(std::size_t current) noexcept -> std::size_t
    {
        return (current + 1) % N;
    }

public:
    ring_buffer()
        : m_head{ 0 }
        , m_tail{ 0 }
    {
    }

    ring_buffer(ring_buffer const&) = default;
    ring_buffer(ring_buffer&&) noexcept = default;
    ~ring_buffer() noexcept = default;

    auto operator=(ring_buffer const&) -> ring_buffer& = default;
    auto operator=(ring_buffer&&) noexcept -> ring_buffer& = default;

    auto push(T const& value) -> bool
    {
        std::size_t head = m_head.load(std::memory_order_relaxed);
        std::size_t next_head = next(head);

        // buffer full
        if(next_head == m_tail.load(std::memory_order_acquire)) {
            return false;
        }

        m_ring.at(head) = value;
        m_head.store(next_head, std::memory_order_release);
        return true;
    }

    auto pop(T& value) -> bool
    {
        std::size_t tail = m_tail.load(std::memory_order_relaxed);

        // buffer empty
        if(tail == m_head.load(std::memory_order_acquire)) {
            return false;
        }

        value = std::move(m_ring.at(tail));
        m_tail.store(next(tail), std::memory_order_release);
        return true;
    }
};

} // namespace gol

#endif // !GOL_THREAD_RING_BUFFER_HPP
