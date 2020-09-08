#ifndef GOL_THREAD_THREADPOOL_HPP
#define GOL_THREAD_THREADPOOL_HPP
#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace gol {

class threadpool
{
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stop{ false };

public:
    threadpool(threadpool const&) = delete;
    threadpool(threadpool&&) = delete;
    ~threadpool() noexcept;

    explicit threadpool(std::size_t num_threads = std::thread::hardware_concurrency());

    auto operator=(threadpool const&) -> threadpool& = delete;
    auto operator=(threadpool &&) -> threadpool& = delete;

    template<typename F, typename... Args>
    auto push(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
    {
        using T = std::invoke_result_t<F, Args...>;

        auto task =
            std::make_shared<std::packaged_task<T()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<T> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock{ m_mutex };

            if(m_stop) {
                throw std::runtime_error{ "Attempted to push to a terminated thread pool!" };
            }

            m_tasks.emplace([task] { (*task)(); });
        }

        m_cv.notify_one();
        return result;
    }
};

} // namespace gol

#endif // !GOL_THREAD_THREADPOOL_HPP
