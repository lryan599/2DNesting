#include "algorithm.h"

namespace nesting {
namespace geo {
FT pwh_area(const Polygon_with_holes_2& p) {
  FT area = 0;
  auto& outer = p.outer_boundary();
  area += outer.area();

  for (auto& hole : p.holes()) {
    area -= hole.area();
  }
  return area;
}

Point_2 find_bottom_left_vertex(const Polygon_2& p) {
  FT ymin = INF;
  FT xmin = INF;
  Point_2 target;
  for (auto v = p.vertices_begin(); v != p.vertices_end(); v++) {
    auto vy = v->y();
    auto vx = v->x();
    if (vy < ymin) {
      ymin = vy;
      xmin = vx;
      target = *v;
    } else if (vy == ymin) {
      if (vx < xmin) {
        xmin = vx;
        target = *v;
      }
    }
  }
  return target;
}

void simplify_polygon(Polygon_2& p) {
  auto i = p.begin();
  auto j = i + 1;
  auto k = j + 1;
  while (j != p.end()) {
    if (CGAL::collinear(*i, *j, *k)) {
      p.erase(j);
    } else {
      i++;
      j++;
      k++;
    }
    if (k == p.end()) {
      k = p.begin();
    }
  }
}

void strict_simplify(Polygon_with_holes_2& p) {
  simplify_polygon(p.outer_boundary());
  for (auto& h : p.holes()) {
    simplify_polygon(h);
  }
}

bool is_translated(const Polygon_2& A, const Polygon_2& B) {
  if (A == B) {
    return true;
  }
  FT ax = 0, ay = 0, bx = 0, by = 0;
  for (auto v = A.vertices_begin(); v != A.vertices_end(); v++) {
    ax += v->x();
    ay += v->y();
  }
  for (auto v = B.vertices_begin(); v != B.vertices_end(); v++) {
    bx += v->x();
    by += v->y();
  }
  FT size((int)A.size());
  Vector_2 v((ax - bx) / size, (ay - by) / size);
  Transformation t(CGAL::TRANSLATION, v);
  auto translated = CGAL::transform(t, B);
  return A == translated;
}

bool is_translated(const Polygon_with_holes_2& A,
                   const Polygon_with_holes_2& B) {
  if (A == B) {
    return true;
  }
  // 只判断A和B的边界，因为B由A旋转得到，内部孔洞均一致
  return is_translated(A.outer_boundary(), B.outer_boundary());
}

Polygon_with_holes_2 transform_polygon_with_holes(
    const Transformation transformation,
    const Polygon_with_holes_2& pgn) {
  // Step1 对边界做变形
  auto boundary = transform(transformation, pgn.outer_boundary());
  std::vector<Polygon_2> holes;
  // Step2 对孔做变形
  for (auto p = pgn.holes_begin(); p != pgn.holes_end(); ++p) {
    holes.push_back(transform(transformation, *p));
  }
  // 返回变形好的带孔多边形
  return Polygon_with_holes_2(boundary, holes.begin(), holes.end());
}

bool is_valid_polygon(const Polygon_2& pgn) {
  return CGAL::is_valid_polygon(pgn, traits);
}

bool is_valid_polygon_with_holes(const Polygon_with_holes_2& pwh) {
  return CGAL::is_valid_polygon_with_holes(pwh, traits);
}

FT comp_pd(const Polygon_with_holes_2& nfp, const Point_2& p) {
  auto& outer = nfp.outer_boundary();
  auto res = CGAL::bounded_side_2(outer.begin(), outer.end(), p);
  // 在nfp外部或边界上
  if (res <= CGAL::ON_BOUNDARY) {
    return FT_ZERO;
  }
  auto& outer_bound = nfp.outer_boundary();
  FT pd = INF;
  auto holes_end = nfp.holes_end();
  for (auto h = nfp.holes_begin(); h != holes_end; h++) {
    CGAL::Bounded_side r = CGAL::bounded_side_2(h->begin(), h->end(), p);
    // 在nfp孔内
    if (r == CGAL::ON_BOUNDED_SIDE) {
      return FT_ZERO;
    }
    // 不在nfp孔内
    auto edges_end = h->edges_end();
    for (auto e = h->edges_begin(); e != edges_end; e++) {
      auto distance = CGAL::squared_distance(*e, p);
      pd = distance < pd ? distance : pd;
    }
  }
  auto edges_end = outer_bound.edges_end();
  // There is no better way to compute the nearest edge so far.
  // Have Tried: Voronoi Diagram in CGAL
  for (auto e = outer_bound.edges_begin(); e != edges_end; e++) {
    auto distance = CGAL::squared_distance(*e, p);
    pd = distance < pd ? distance : pd;
  }
  return pd;
}

Polygon_2 comp_ifr(const Polygon_2 poly_A, const Polygon_2 poly_B) {
  auto bbox_A = poly_A.bbox();

  auto first_vertex = poly_B.vertices_begin();
  Transformation translate(CGAL::TRANSLATION,
                           Vector_2(-first_vertex->x(), -first_vertex->y()));
  auto shifted_B = transform(translate, poly_B);
  auto bbox_B = shifted_B.bbox();

  // Sheet装不下
  if ((bbox_A.x_span()) < (bbox_B.x_span()) ||
      (bbox_A.y_span()) < (bbox_B.y_span())) {
    return Polygon_2();
  }

  Polygon_2 outer_boundary;
  auto x1 = bbox_A.xmin() - bbox_B.xmin();
  auto x2 = bbox_A.xmax() - bbox_B.xmax();
  auto y1 = bbox_A.ymin() - bbox_B.ymin();
  auto y2 = bbox_A.ymax() - bbox_B.ymax();
  // 去除重复的点，IFR可能退化成线或点
  if (x1 == x2) {
    outer_boundary.push_back(Point_2(x1, y1));
    if (y1 != y2) {
      outer_boundary.push_back(Point_2(x1, y2));
    }
  } else {
    outer_boundary.push_back(Point_2(x1, y1));
    outer_boundary.push_back(Point_2(x2, y1));
    if (y1 != y2) {
      outer_boundary.push_back(Point_2(x2, y2));
      outer_boundary.push_back(Point_2(x1, y2));
    }
  }
  return outer_boundary;
}

Polygon_2 comp_ifr(const Polygon_with_holes_2 poly_A,
                   const Polygon_with_holes_2 poly_B) {
  auto bbox_A = poly_A.bbox();

  auto first_vertex = poly_B.outer_boundary().vertices_begin();
  Transformation translate(CGAL::TRANSLATION,
                           Vector_2(-first_vertex->x(), -first_vertex->y()));
  auto shifted_B = transform(translate, poly_B.outer_boundary());
  auto bbox_B = shifted_B.bbox();

  // Sheet装不下
  if ((bbox_A.x_span()) < (bbox_B.x_span()) ||
      (bbox_A.y_span()) < (bbox_B.y_span())) {
    return Polygon_2();
  }

  Polygon_2 outer_boundary;
  auto x1 = bbox_A.xmin() - bbox_B.xmin();
  auto x2 = bbox_A.xmax() - bbox_B.xmax();
  auto y1 = bbox_A.ymin() - bbox_B.ymin();
  auto y2 = bbox_A.ymax() - bbox_B.ymax();
  // 去除重复的点，IFR可能退化成线或点
  if (x1 == x2) {
    outer_boundary.push_back(Point_2(x1, y1));
    if (y1 != y2) {
      outer_boundary.push_back(Point_2(x1, y2));
    }
  } else {
    outer_boundary.push_back(Point_2(x1, y1));
    outer_boundary.push_back(Point_2(x2, y1));
    if (y1 != y2) {
      outer_boundary.push_back(Point_2(x2, y2));
      outer_boundary.push_back(Point_2(x1, y2));
    }
  }
  return outer_boundary;
}

void offset_polygon(Polygon_with_holes_2& p, double offset) {
  if (offset == 0) {
    return;
  }
  // 只对外边界做膨胀/收缩
  // 合法的outer是逆时针
  auto& outer = p.outer_boundary();
  if (offset > 0) {
    auto ps = CGAL::create_exterior_skeleton_and_offset_polygons_2(offset,
                                                                   outer, K());
    if (ps.size() != 2) {
      throw std::runtime_error("Error offset_polygon(): ps.size() != 2");
    }
    outer = *(ps[1].get());
  } else {
    auto ps = CGAL::create_interior_skeleton_and_offset_polygons_2(-offset,
                                                                   outer, K());
    if (ps.size() != 1) {
      throw std::runtime_error("Error offset_polygon(): ps.size() != 1");
    }
    outer = *(ps[0].get());
  }
  // 保持outer为逆时针
  if (outer.is_clockwise_oriented()) {
    outer.reverse_orientation();
  }
}

bool simplify(Polygon_2& p) {
  if (p.is_convex()) {
    return false;
  }
  double factor = 0.02;
  double minimum = INF;
  for (auto e = p.edges_begin(); e != p.edges_end(); e++) {
    minimum = (std::min)(minimum, CGAL::to_double(e->squared_length()));
  }
  Simplification::Convex_squared_distance_cost cost;
  Simplification::Stop_above_cost_threshold stop(factor * minimum);
  Polygon_2 q(p);
  for (size_t i = 0; i < 10; i++) {
    q = Simplification::simplify(q, cost, stop);
    std::vector<Polygon_with_holes_2> t;
    CGAL::intersection(p, q, std::back_inserter(t));
    // It should be only one polygon
    size_t size = t.size();
    if (size != 1) {
      throw std::runtime_error("Error simplify(): size != 1");
    }
    auto& m = t[0];
    // p intersects q should equal to p
    if (m != p) {
      CGAL::draw(p);
      CGAL::draw(m);
      throw std::runtime_error("Error simplify(): p intersects q != p");
    }
  }
  if (p.size() != q.size()) {
    std::clog << "Polygon Simplify: " << std::endl;
    std::clog << "Before: \n" << p << std::endl;
    std::clog << "After: \n" << q << "\n" << std::endl;
    p = q;
    return true;
  }
  return false;
}

bool simplify(Polygon_with_holes_2& p) {
  bool flag = false;
  auto& outer = p.outer_boundary();
  flag |= simplify(outer);
  for (auto& pgn : p.holes()) {
    flag |= simplify(pgn);
  }
  return flag;
}

}  // namespace geo
}  // namespace nesting