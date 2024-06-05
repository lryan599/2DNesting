#pragma once

#include "nesting.h"
#include "postprocess.h"
#include "preprocess.h"

namespace nesting {

struct Preprocess {
  Layout layout;
  std::vector<nesting::Item> original_items;
  std::vector<nesting::Item> simplified_items;
  Preprocess(Layout& _layout,
             std::vector<nesting::Item>& _original_items,
             std::vector<nesting::Item>& _simplified_items)
      : layout(_layout) {
    original_items.swap(_original_items);
    simplified_items.swap(_simplified_items);
  }
};
Preprocess preprocess(
    const bool need_simplify,
    const double top_offset,
    const double left_offset,
    const double bottom_offset,
    const double right_offset,
    const double part_offset,
    const double sheet_width,
    const double sheet_height,
    const std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
    const std::vector<uint32_t>& items_rotations,
    const std::vector<uint32_t>& items_quantity);
void start(const size_t max_time,
           Layout& layout,
           std::function<void(const Solution&)> ProgressHandler,
           volatile bool* requestQuit);
}  // namespace nesting
