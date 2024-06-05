#include "nesting.h"

namespace nesting {
NFPCacheValue& comp_nfp(const Polygon_with_holes_2* poly_A,
                        const uint32_t rotation_A,
                        const uint32_t allowed_rotation_A,
                        const Polygon_with_holes_2* poly_B,
                        const uint32_t rotation_B,
                        const uint32_t allowed_rotation_B,
                        Layout& layout) {
  NFPCacheKey key(poly_A, poly_B, rotation_A, rotation_B);
  Polygon_with_holes_2 nfp;
  auto kv = layout.nfp_cache.find(key);
  if (kv == layout.nfp_cache.end()) {
    Transformation scale(CGAL::SCALING, -1);
    auto rotate_A = geo::get_rotate(rotation_A, allowed_rotation_A);
    auto rotate_B = geo::get_rotate(rotation_B, allowed_rotation_B);
    Polygon_with_holes_2 minus_B;
    Polygon_with_holes_2 rotated_A;
    if (rotate_B) {
      minus_B = geo::transform_polygon_with_holes(scale * (*rotate_B), *poly_B);
    } else {
      minus_B = geo::transform_polygon_with_holes(scale, *poly_B);
    }
    if (rotate_A) {
      rotated_A = geo::transform_polygon_with_holes(*rotate_A, *poly_A);
    } else {
      rotated_A = *poly_A;
    }
    nfp = CGAL::minkowski_sum_2(rotated_A, minus_B);
    geo::strict_simplify(nfp);
    auto bbox = nfp.bbox();
    NFPCacheValue v;
    v.nfp = nfp;
    v.xmin = CGAL::to_double(bbox.xmin());
    v.xmax = CGAL::to_double(bbox.xmax());
    v.ymin = CGAL::to_double(bbox.ymin());
    v.ymax = CGAL::to_double(bbox.ymax());
    auto _kv = layout.nfp_cache.emplace(key, v);
    return _kv.first->second;
  } else {
    return kv->second;
  }
}
FT comp_pd(NFPCacheValue& v, double px, double py, Layout& layout) {
  auto& original_nfp = v.nfp;
  if (px <= v.xmin || px >= v.xmax || py <= v.ymin || py >= v.ymax) {
    return geo::FT_ZERO;
  }
  hash::PDCacheKey key(&original_nfp, px, py);
  FT pd;
  layout.pd_count++;
  auto iter = layout.pd_cache.find(key);
  if (iter != layout.pd_cache.end()) {
    pd = iter->second;
  } else {
    layout.pd_miss++;
    // 为效率考虑使用不精确构建
    Point_2 relative_point(px, py);
    pd = geo::comp_pd(original_nfp, relative_point);
    layout.pd_cache.emplace(key, pd);
  }
  return pd;
}
void shrink(Layout& layout) {
  layout.cur_length = layout.best_length * (1 - layout.rdec);
  if (layout.cur_length < layout.lower_length) {
    layout.cur_length = layout.lower_length;
  }
  std::vector<size_t> v;
  // 将超出cur_length的polygon重新放置
  for (size_t i = 0; i < layout.poly_num; i++) {
    auto& p = layout.sheet_parts[0][i];
    auto right_vertex = p.transformed.outer_boundary().right_vertex();
    auto x = right_vertex->x();
    if (x > layout.cur_length) {
      auto ifr = geo::comp_ifr(layout.sheets[0].sheet, p.transformed);
      auto bbox = ifr.bbox();
      double random_x;
      double random_y;
      // 放置策略：随机放置
      random_x = bbox.x_span() * rand::right_nd01() + bbox.xmin();
      random_y = bbox.y_span() * rand::center_nd01() + bbox.ymin();
      p.set_translate(random_x, random_y);
      v.push_back(i);
    }
  }
  // 更新pd
  for (auto& i : v) {
    auto& p = layout.sheet_parts[0][i];
    auto& px = p.get_translate_ft_x();
    auto& py = p.get_translate_ft_y();
    auto double_px = CGAL::to_double(px);
    auto double_py = CGAL::to_double(py);
    for (size_t j = 0; j < layout.poly_num; j++) {
      if (i == j) {
        continue;
      }
      auto& q = layout.sheet_parts[0][j];
      auto& qx = q.get_translate_ft_x();
      auto& qy = q.get_translate_ft_y();
      auto double_qx = q.get_translate_double_x();
      auto double_qy = q.get_translate_double_y();
      auto& nfp =
          comp_nfp(q.base, q.get_rotation(), q.allowed_rotations, p.base,
                   p.get_rotation(), p.allowed_rotations, layout);
      Point_2 relative_point(px - qx, py - qy);
      auto pd =
          comp_pd(nfp, double_px - double_qx, double_py - double_qy, layout);
      layout.set_pd(i, j, pd);
    }
  }
  // 更新sheet
  layout.sheets[0].set_width(layout.cur_length);
}
void get_init_solu(Layout& layout) {
  //std::ofstream s("init.txt", std::ios::out);
  //s << "init solu start" << std::endl;
  size_t num_poly = layout.poly_num;
  for (size_t i = 0; i < num_poly; i++) {
    auto& shape_i = layout.sheet_parts[0][i];
    auto& polygon_i = shape_i.transformed;
    auto rotation_i = shape_i.get_rotation();
    auto& ix = shape_i.get_translate_ft_x();
    auto& iy = shape_i.get_translate_ft_y();
    auto double_ix = CGAL::to_double(ix);
    auto double_iy = CGAL::to_double(iy);
    auto ifr = geo::comp_ifr(layout.sheets[0].sheet, polygon_i);
    CandidatePoints c(layout.poly_num - 1);
    c.set_boundary(ifr);
    for (size_t j = 0; j < num_poly; j++) {
      if (j == i) {
        continue;
      }
      auto& shape_j = layout.sheet_parts[0][j];
      auto& jx = shape_j.get_translate_ft_x();
      auto& jy = shape_j.get_translate_ft_y();
      auto double_jx = shape_j.get_translate_double_x();
      auto double_jy = shape_j.get_translate_double_y();
      auto translate = shape_j.get_translate();
      auto rotation_j = shape_j.get_rotation();
      auto& nfp =
          comp_nfp(shape_j.base, rotation_j, shape_j.allowed_rotations,
                   shape_i.base, rotation_i, shape_i.allowed_rotations, layout);
      FT pd;
      // j < i时的pd已经被更新
      if (j > i) {
        Point_2 relative_point(ix - jx, iy - jy);

        pd = comp_pd(nfp, double_ix - double_jx, double_iy - double_jy, layout);
        layout.set_pd(i, j, pd);
      }
      c.nfps.push_back(&nfp);
      c.translations.push_back(translate);
      c.translate_x.push_back(double_jx);
    }
    c.initialize();
    auto points = c.get_perfect_points();
    // points为空时
    if (points.empty()) {
      std::cerr << "Error: Candidate points set is empty" << std::endl;
      break;
    }
    auto& first_point = points[0];
    auto first_x = first_point.x();
    auto first_y = first_point.y();
    auto double_first_x = CGAL::to_double(first_x);
    auto double_first_y = CGAL::to_double(first_y);
    for (size_t j = 0; j < num_poly; j++) {
      size_t k = j;
      if (i == j) {
        continue;
      } else if (j > i) {
        k--;
      }
      auto& nfp = c.nfps[k];
      // auto& t = c.translations[k];
      auto& shape_j = layout.sheet_parts[0][j];
      auto& jx = shape_j.get_translate_ft_x();
      auto& jy = shape_j.get_translate_ft_y();
      auto double_jx = shape_j.get_translate_double_x();
      auto double_jy = shape_j.get_translate_double_y();
      Point_2 relative_point(first_x - jx, first_y - jy);
      // 通常情况下此时pd为0
      auto pd = comp_pd(*nfp, double_first_x - double_jx,
                        double_first_y - double_jy, layout);
      shape_i.set_translate(first_x, first_y);
      layout.set_pd(i, j, pd);
    }
  }
  // 判断初始解是否可行
  auto pure_overlap = layout.get_pure_total_pd();
  if (pure_overlap > geo::BIAS) {
    throw std::runtime_error("Error get_init_solu(): initial solution is not feasible");
  }
  // 更新layout当前长度
  //s << "layout update" << std::endl;
  layout.update_cur_length();
  layout.best_length = layout.cur_length;
  // 更新sheet
  layout.sheets[0].set_width(layout.cur_length);
  //s << "update best result" << std::endl;
  layout.best_result = layout.sheet_parts;
  //s << "update best util" << std::endl;
  layout.best_utilization = CGAL::to_double(
      layout.area / (layout.best_length * layout.sheets[0].height));
  //s << "init solu end" << std::endl;
}
bool minimize_overlap(Layout& layout, volatile bool* requestQuit) {
  size_t numIterations = 0;
  FT minOverlap = geo::INF;
  layout.initialize_miu();
  std::clog << "It: ";
  std::vector<size_t> indices;
  for (size_t i = 0; i < layout.poly_num; i++) {
    indices.push_back(i);
  }
  while (numIterations < layout.maxIterations) {
    if (*requestQuit) {
      return false;
    }
    //  对polygon随机排列
    std::shuffle(indices.begin(), indices.end(), rand::random_engine3);
    // 随机选一个polygon，寻找它可能存在的最小位置
    for (size_t i = 0; i < layout.poly_num; i++) {
      auto idx = indices[i];
      auto& shape = layout.sheet_parts[0][idx];
      auto cur_pd = layout.get_one_polygon_pd(idx);
      // 如果这个polygon与其他polygon没有发生重叠，直接跳过
      if (cur_pd < geo::BIAS && cur_pd > -geo::BIAS) {
        continue;
      }
      // 遍历polygon的所有角度
      for (auto& rotation : shape.reduced_rotations) {
        auto rotate = geo::get_rotate(rotation, shape.allowed_rotations);
        Polygon_with_holes_2 rotated;
        if (rotate) {
          rotated = geo::transform_polygon_with_holes(*rotate, *shape.base);
        } else {
          rotated = *shape.base;
        }
        // 计算候选点
        CandidatePoints c(layout.poly_num - 1);
        c.set_boundary(geo::comp_ifr(layout.sheets[0].sheet, rotated));
        for (size_t k = 0; k < layout.poly_num; k++) {
          if (k == idx) {
            continue;
          }
          auto& shape_k = layout.sheet_parts[0][k];
          auto& nfp = comp_nfp(shape_k.base, shape_k.get_rotation(),
                               shape_k.allowed_rotations, shape.base, rotation,
                               shape.allowed_rotations, layout);
          auto translate_x = shape_k.get_translate_double_x();
          c.nfps.push_back(&nfp);
          c.translations.push_back(shape_k.get_translate());
          c.translate_x.push_back(translate_x);
        }
        c.initialize();
        auto points = c.get_arrangement_points();
        auto size = points.size();
        // std::clog << "The size of Candidate points is: " << size <<
        // std::endl;
        //  TODO 这里可以压缩到多线程求最小值的规约问题->求min(new_pd)
        for (size_t l = 0; l < size; ++l) {
          auto& point = points[l];
          auto point_x = point.x();
          auto point_y = point.y();
          auto double_point_x = CGAL::to_double(point_x);
          auto double_point_y = CGAL::to_double(point_y);
          double new_pd = 0;
          std::vector<FT> temp(layout.poly_num - 1);
          // 尝试将polygon idx平移到候选点上去，计算平移到哪个点polygon
          // idx与其他多边形的pd之和会降低
          for (size_t k = 0; k < layout.poly_num - 1; k++) {
            size_t m = k;
            if (k >= idx) {
              m = k + 1;
            }
            auto& nfp = c.nfps[k];
            auto& shape_m = layout.sheet_parts[0][m];
            auto double_mx = shape_m.get_translate_double_x();
            auto double_my = shape_m.get_translate_double_y();
            temp[k] = comp_pd(*nfp, double_point_x - double_mx,
                              double_point_y - double_my, layout);
            auto t = CGAL::to_double(temp[k]);
            new_pd += t * layout.get_miu(idx, m);
          }
          // 选择使得pd更小的那个点，如果不存在这样的点，则不会对layout发生任何变化
          if (new_pd < cur_pd) {
            shape.set(rotation, point_x, point_y);
            for (size_t k = 0; k < layout.poly_num - 1; k++) {
              if (k < idx) {
                layout.set_pd(idx, k, temp[k]);
              } else {
                layout.set_pd(idx, k + 1, temp[k]);
              }
            }
            cur_pd = new_pd;
          }
          // 如果这个候选点对其他polygon没有重叠，直接返回
          if (cur_pd < geo::BIAS) {
            break;
          }
        }
      }
    }
    auto pure_overlap = layout.get_pure_total_pd();
    // auto overlap = layout.get_total_pd();
    if (pure_overlap < geo::BIAS) {
      std::clog << std::endl;
      return true;
    } else if (pure_overlap < minOverlap) {
      std::clog << std::endl;
      std::clog << "Overlap: " << pure_overlap << std::endl;
      std::clog << "Total/Miss: " << layout.pd_count << "/" << layout.pd_miss
                << ", " << (double)layout.pd_miss / layout.pd_count
                << std::endl;
      std::clog << "It: ";
      minOverlap = pure_overlap;
      numIterations = 0;
    }
    layout.update_miu();
    numIterations++;
  }
  std::clog << std::endl;
  return false;
}
void GOMH(Layout& layout,
          size_t max_time,
          std::function<void(const Solution&)> ProgressHandler,
          volatile bool* requestQuit) {
  clock_t start = clock();
  get_init_solu(layout);
  ProgressHandler(Solution(
      CGAL::to_double(layout.best_length), layout.best_utilization,
      ((double)(clock() - start) / CLOCKS_PER_SEC), layout.best_result[0]));
  shrink(layout);
  while (((double)(clock() - start) / CLOCKS_PER_SEC) < max_time) {
    if (*requestQuit) {
      return;
    }
    auto feasible = minimize_overlap(layout, requestQuit);
    if (feasible) {
      layout.best_result = layout.sheet_parts;
      layout.best_length = (std::min)(layout.best_length, layout.cur_length);
      layout.best_utilization = CGAL::to_double(
          layout.area / (layout.best_length * layout.sheets[0].height));
      ProgressHandler(Solution(
          CGAL::to_double(layout.best_length), layout.best_utilization,
          ((double)(clock() - start) / CLOCKS_PER_SEC), layout.best_result[0]));
      // print_result(layout, start);
      if (layout.best_length <= layout.lower_length) {
        break;
      }
      shrink(layout);
    } else {
      auto t = layout.cur_length * (1 + layout.rinc);
      if (t >= layout.best_length) {
        shrink(layout);
      } else {
        expand(layout);
      }
    }
  }
  std::clog << "best length: " << layout.best_length << std::endl;
  std::clog << "best utilization rate: " << layout.best_utilization
            << std::endl;
}
}  // namespace nesting
