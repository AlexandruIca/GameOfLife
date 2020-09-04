#include "coord.hpp"

#include <tuple>

#define OP(op)                                                                                                         \
    auto operator op(coord const& a, coord const& b) noexcept->bool                                                    \
    {                                                                                                                  \
        return std::tie(a.x, a.y) op std::tie(b.x, b.y);                                                               \
    }

namespace gol {

OP(==)
OP(!=)
OP(<=)
OP(<)
OP(>)
OP(>=)

} // namespace gol

#undef OP
