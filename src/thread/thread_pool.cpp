#include "thread_pool.hpp"

namespace gol {

threadpool::threadpool(std::size_t const num_threads)
{
    m_workers.reserve(num_threads);

    for(std::size_t i = 0; i < num_threads; ++i) {
        m_workers.emplace_back([this]() -> void {
            for(;;) {
                std::function<void()> task{};
                {
                    std::unique_lock<std::mutex> lock{ m_mutex };
                    m_cv.wait(lock, [this] { return m_stop || !m_tasks.empty(); });

                    if(m_stop && m_tasks.empty()) {
                        return;
                    }

                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                task();
            }
        });
    }
}

threadpool::~threadpool() noexcept
{
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        m_stop = true;
    }

    m_cv.notify_all();

    for(auto& thread : m_workers) {
        thread.join();
    }
}

} // namespace gol
