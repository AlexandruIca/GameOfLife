#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <algorithm>
#include <iostream>

#include "thread/ring_buffer.hpp"
#include "thread/thread_pool.hpp"

TEST_CASE("Basic RingBuffer Test")
{
    constexpr int N = 11;
    gol::ring_buffer<int, N> v;

    REQUIRE(v.push(0));
    REQUIRE(v.push(1));
    REQUIRE(v.push(2));
    REQUIRE(v.push(3));
    REQUIRE(v.push(4));
    REQUIRE(v.push(5));
    REQUIRE(v.push(6));
    REQUIRE(v.push(7));
    REQUIRE(v.push(8));
    REQUIRE(v.push(9));
    REQUIRE(!v.push(10));

    int val = 0;

    for(int i = 0; i < N - 1; ++i) {
        REQUIRE(v.pop(val));
        REQUIRE(val == i);
    }
}

TEST_CASE("RingBuffer + ThreadPool")
{
    constexpr int N = 11;
    gol::ring_buffer<int, N> v;

    {
        gol::threadpool tp{ 1 };
        std::vector<std::future<void>> futures;

        futures.reserve(N);

        for(int i = 0; i < N - 1; ++i) {
            try {
                futures.push_back(tp.push([&v, i] { v.push(i + 1); }));
            }
            catch(std::exception const& e) {
                MESSAGE("Exception: " << e.what());
            }
        }

        for(auto& f : futures) {
            f.get();
        }
    }

    std::vector<int> numbers;
    numbers.reserve(N);

    for(int i = 0; i < N - 1; ++i) {
        int val = 0;
        REQUIRE(v.pop(val));
        numbers.push_back(val);
    }

    std::sort(numbers.begin(), numbers.end());

    for(std::size_t i = 0; i < N - 1; ++i) {
        REQUIRE(numbers[i] == i + 1);
    }
}
