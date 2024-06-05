#pragma once
#include "algorithm.h"
#include "part.h"
#include "transformed_shape.h"

namespace nesting {
std::vector<Polygon_with_holes_2> postprocess(
    std::vector<TransformedShape>& solution,
    double offset,
    double left_offset,
    double bottom_offset,
    const std::vector<Item>& simplified_items,
    const std::vector<Item>& original_items);
}  // namespace nesting