#include "postprocess.h"

namespace nesting {
std::vector<Polygon_with_holes_2> postprocess(
    std::vector<TransformedShape>& solution,
    double offset,
    double left_offset,
    double bottom_offset,
    const std::vector<Item>& simplified_items,
    const std::vector<Item>& original_items) {
  std::vector<Polygon_with_holes_2> res;
  res.reserve(solution.size());
  for (auto& t : solution) {
    // Step1 将所有多边形平移(left_offset, bottom_offset)
    if (left_offset != 0 || bottom_offset != 0) {
      t.set_translate(t.get_translate_ft_x() + left_offset,
                      t.get_translate_ft_y() + bottom_offset);
    }
    if (offset == 0 && !t.simplified) {
      res.push_back(t.transformed);
      continue;
    }
    // Step2 复原膨胀的多边形
    auto rotate = geo::get_rotate(t.get_rotation(), t.allowed_rotations);
    auto translate = t.get_translate();
    Polygon_with_holes_2 simplified_pwh(simplified_items[t.item_idx].poly);
    if (offset != 0) {
      if (rotate) {
        simplified_pwh = geo::transform_polygon_with_holes(
            translate * (*rotate), simplified_pwh);
      } else {
        simplified_pwh =
            geo::transform_polygon_with_holes(translate, simplified_pwh);
      }
    }
    // Step3 判断是否需要复原简化，需要就复原简化的多边形
    if (t.simplified) {
      Polygon_with_holes_2 original_pwh(original_items[t.item_idx].poly);
      if (rotate) {
        original_pwh = geo::transform_polygon_with_holes(*rotate, original_pwh);
      }
      auto& outer1 = original_pwh.outer_boundary();
      auto p1 = geo::find_bottom_left_vertex(outer1);
      auto& outer0 = (offset != 0) ? simplified_pwh.outer_boundary()
                                   : t.transformed.outer_boundary();
      auto p0 = geo::find_bottom_left_vertex(outer0);
      Transformation tr(CGAL::TRANSLATION,
                        Vector_2(p0.x() - p1.x(), p0.y() - p1.y()));
      original_pwh = geo::transform_polygon_with_holes(tr, original_pwh);
      res.push_back(original_pwh);
    } else {
      res.push_back(simplified_pwh);
    }
  }
  return res;
}
}  // namespace nesting
