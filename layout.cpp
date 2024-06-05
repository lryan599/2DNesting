#include "layout.h"

namespace nesting {

Layout::Layout(std::vector<Item> items, std::vector<Sheet> _sheets, FT _area) {
  sheet_parts.resize(_sheets.size());
  sheets.swap(_sheets);
  // 如果不指定面积，则进行计算。这里计算的已经是前处理后的总面积。
  bool flag = false;
  if (_area == -1) {
    area = 0;
    flag = true;
  } else {
    area = _area;
  }
  auto size = items.size();
  double max_x_span = 0;
  for (size_t i = 0; i < size; i++) {
    auto& item = items[i];
    auto& n = item.quantity;
    auto& p = item.poly;
    max_x_span = std::max(max_x_span, p.bbox().x_span());
    auto& rotations = item.rotations;
    auto first = p.outer_boundary().vertices_begin();
    Transformation shift_to_zero(CGAL::TRANSLATION,
                                 Vector_2(-first->x(), -first->y()));
    auto transformed = geo::transform_polygon_with_holes(shift_to_zero, p);
    poly_num += n;
    if (flag) {
      area += FT(n) * geo::pwh_area(p);
    }
    auto canonical_hole = get_canonical_polygon(transformed);
    for (size_t j = 0; j < n; j++) {
      // TODO 暂时按单板来做
      sheet_parts[0].emplace_back(canonical_hole, first->x(), first->y(), 0,
                                  rotations, item.simplified, i);
    }
  }
  // best_result.resize(sheet_parts.size());
  glob_pd.resize(poly_num * (poly_num - 1) / 2);
  miu.resize(poly_num * (poly_num - 1) / 2);
  // 初始化pd_cache
  pd_cache.init(max_pd_cache);
  pd_cache.reserve(max_pd_cache);

  std::clog << "total_area: " << area << std::endl;
  std::clog << sheets[0].height << std::endl;
  FT min_length(max_x_span);
  lower_length = std::max(area / sheets[0].height, min_length);
  //lower_length = area / sheets[0].height;
}
Polygon_with_holes_2* Layout::get_canonical_polygon(
    const Polygon_with_holes_2& poly) {
  // 如果polygon_cache上不存在则注册
  auto iter = canonical_polygons.find(poly);
  if (iter == canonical_polygons.end()) {
    auto res = std::make_shared<Polygon_with_holes_2>(poly);
    canonical_polygons[poly] = res;
    return res.get();
  } else {
    return iter->second.get();
  }
}
void Layout::update_miu() {
  FT max_pd = 0;
  auto size = miu.size();
  for (size_t i = 0; i < size; i++) {
    max_pd = (std::max)(max_pd, glob_pd[i]);
  }
  if (max_pd == 0) {
    return;
  }
  for (size_t i = 0; i < size; i++) {
    miu[i] += CGAL::to_double(glob_pd[i] / max_pd);
  }
}
void Layout::update_cur_length() {
  FT length = 0;
  for (auto& p : this->sheet_parts[0]) {
    length =
        (std::max)(length, p.transformed.outer_boundary().right_vertex()->x());
  }
  this->cur_length = length;
}
void Layout::debug_glob_pd() {
  std::clog << "glob_pd: ";
  for (size_t i = 0; i < glob_pd.size(); i++) {
    std::clog << i << ":" << glob_pd[i] << " ";
  }
  std::clog << std::endl;
}
}  // namespace nesting
