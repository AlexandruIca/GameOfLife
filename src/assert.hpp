#ifndef GOL_ASSERT_HPP
#define GOL_ASSERT_HPP
#pragma once

#include <cstdlib>

#include "log.hpp"

#ifdef GOL_DEBUG

#define ASSERT(...)                                                                                                    \
    if(!(__VA_ARGS__)) {                                                                                               \
        FATAL("Assertion failed at {}[{}]: {}", __FILE__, __LINE__, #__VA_ARGS__);                                     \
        std::exit(EXIT_FAILURE);                                                                                       \
    }                                                                                                                  \
    static_cast<void>(0)

#define ASSERT_MSG(cond, msg)                                                                                          \
    if(!(cond)) {                                                                                                      \
        FATAL("Assertion failed at {}[{}]: {}", __FILE__, __LINE__, msg);                                              \
        std::exit(EXIT_FAILURE);                                                                                       \
    }                                                                                                                  \
    static_cast<void>(0)

#else

#define ASSERT(...) static_cast<void>(0)
#define ASSERT_MSG(...) static_cast<void>(0)

#endif

#endif // !GOL_ASSERT_HPP
