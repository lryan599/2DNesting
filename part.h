#pragma once
#include <cstdint>
#include "basic.h"
namespace nesting {
struct Item {
  uint32_t quantity = 0;
  geo::Polygon_with_holes_2 poly;
  uint32_t rotations = 0;
  bool simplified = false;
  explicit Item(uint32_t _quantity,
                geo::Polygon_with_holes_2 _poly,
                uint32_t _rotations)
      : quantity(_quantity), poly(_poly), rotations(_rotations) {}
  explicit Item(geo::Polygon_with_holes_2 _poly) : poly(_poly) {}
};
}  // namespace nesting