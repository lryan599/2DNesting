#include "preprocess.h"

namespace nesting {
namespace geo {
void preprocess(std::vector<nesting::Item>& items,
                double offset,
                bool need_simplify) {
  for (auto& item : items) {
    auto& p = item.poly;
    auto& outer = p.outer_boundary();
    // 外轮廓需要是逆时针方向
    if (outer.is_clockwise_oriented()) {
      outer.reverse_orientation();
    }
    for (auto& h : p.holes()) {
      // 孔洞需要是顺时针方向
      if (h.is_counterclockwise_oriented()) {
        h.reverse_orientation();
      }
    }
    // TODO 设置并捕获异常
    if (!is_valid_polygon_with_holes(p)) {
      throw std::runtime_error("Input polygon is not valid");
    }
    // 简化多边形
    if (need_simplify) {
      item.simplified = simplify(p);
    }
  }
}

void preprocess(Sheet& sheet,
                double top_offset,
                double left_offset,
                double bottom_offset,
                double right_offset) {
  double vertical = 0;
  double horizontal = 0;
  vertical -= top_offset;
  vertical -= bottom_offset;
  horizontal -= left_offset;
  horizontal -= right_offset;
  sheet.set_height(sheet.height + vertical);
  sheet.set_width(sheet.width + horizontal);
}
}  // namespace geo
}  // namespace nesting