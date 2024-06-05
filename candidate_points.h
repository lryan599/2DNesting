#pragma once
#include <CGAL/Surface_sweep_2_algorithms.h>
#include "algorithm.h"
#include "hasher.h"
#include "rand.h"
namespace nesting {
// ------------------------------------------------------------------
//							  生成候选点
// ------------------------------------------------------------------
struct CandidatePoints {
  geo::Polygon_2 boundary;
  bool has_boundary;
  geo::FT xmin;
  geo::FT xmax;
  geo::FT ymin;
  geo::FT ymax;
  std::vector<hash::NFPCacheValue*> nfps;
  std::vector<geo::Transformation> translations;
  std::vector<geo::Polygon_with_holes_2> translated_nfps;
  std::vector<double> translate_x;
  size_t max_candidate_points = 300;
  size_t batch_size = 16;
  size_t size = 0;

  explicit CandidatePoints(size_t _size) : has_boundary(false), size(_size) {
    nfps.reserve(size);
    translated_nfps.reserve(size);
    translations.reserve(size);
    translate_x.reserve(size);
  }

  void initialize(bool sorted = true);

  void set_boundary(const geo::Polygon_2& ifr) {
    boundary = ifr;
    auto bbox = ifr.bbox();
    xmin = bbox.xmin();
    xmax = bbox.xmax();
    ymin = bbox.ymin();
    ymax = bbox.ymax();
    has_boundary = true;
  }

  // 判断点是否在ifr内部
  bool is_valid(const geo::Point_2& p) const {
    auto x = p.x();
    auto y = p.y();
    return x >= xmin && x <= xmax && y >= ymin && y <= ymax;
  }

  std::vector<geo::Point_2> get_perfect_points();

  std::vector<geo::Point_2> get_arrangement_points();
};
}  // namespace nesting
