#ifndef GOL_COORD_HPP
#define GOL_COORD_HPP
#pragma once

namespace gol {

struct coord
{
    int x = 0;
    int y = 0;
};

[[nodiscard]] auto operator==(coord const& a, coord const& b) noexcept -> bool;
[[nodiscard]] auto operator!=(coord const& a, coord const& b) noexcept -> bool;
[[nodiscard]] auto operator<=(coord const& a, coord const& b) noexcept -> bool;
[[nodiscard]] auto operator<(coord const& a, coord const& b) noexcept -> bool;
[[nodiscard]] auto operator>(coord const& a, coord const& b) noexcept -> bool;
[[nodiscard]] auto operator>=(coord const& a, coord const& b) noexcept -> bool;

} // namespace gol

#endif // !GOL_COORD_HPP
