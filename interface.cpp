#include "interface.h"

namespace nesting {

void start(const size_t max_time,
           Layout& layout,
           std::function<void(const Solution&)> ProgressHandler,
           volatile bool* requestQuit) {
  nesting::GOMH(layout, max_time, ProgressHandler, requestQuit);
  //// 后处理
  // auto res = geo::postprocess(layout, part_offset / 2, left_offset,
  //                             bottom_offset, simplified_items,
  //                             original_items);
  // 将res导出
  // nesting::write_to_txt("result.svg", layout.best_utilization,
  //                      original_sheets[0], res);
}
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
    const std::vector<uint32_t>& items_quantity) {
  // 处理original_items
  std::vector<nesting::Item> original_items;
  size_t size = polygons.size();
  for (size_t i = 0; i < size; i++) {
    original_items.emplace_back(items_quantity[i], polygons[i],
                                items_rotations[i]);
  }
  // 计算面积
  FT area(0);
  for (auto& i : original_items) {
    area += geo::pwh_area(i.poly) * FT(i.quantity);
  }
  // 处理sheet
  nesting::Sheet sheet(sheet_width, sheet_height);
  std::vector<nesting::Sheet> original_sheets{sheet};
  // 获取副本
  std::vector<nesting::Sheet> sheets;
  std::vector<nesting::Item> items;
  std::copy(original_items.begin(), original_items.end(),
            std::back_inserter(items));
  std::copy(original_sheets.begin(), original_sheets.end(),
            std::back_inserter(sheets));
  // 前处理
  geo::preprocess(items, part_offset / 2, need_simplify);
  geo::preprocess(sheets[0], top_offset, left_offset, bottom_offset,
                  right_offset);
  std::vector<nesting::Item> simplified_items;
  std::copy(items.begin(), items.end(), std::back_inserter(simplified_items));
  // 多边形膨胀
  for (size_t i = 0; i < items.size(); i++) {
    auto& item = items[i];
    geo::offset_polygon(item.poly, part_offset / 2);
    auto first = item.poly.outer_boundary().vertices_begin();
    Transformation translate(CGAL::TRANSLATION,
                             Vector_2(-first->x(), -first->y()));
    simplified_items[i].poly =
        geo::transform_polygon_with_holes(translate, simplified_items[i].poly);
  }
  // 排样
  nesting::Layout layout(items, sheets, area);
  return Preprocess(layout, original_items, simplified_items);
}
}  // namespace nesting
