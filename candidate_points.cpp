#include "candidate_points.h"
namespace nesting {
void CandidatePoints::initialize(bool sorted) {
  if (sorted) {
    std::vector<int> I(size);
    std::iota(begin(I), end(I), 0);
    std::sort(begin(I), end(I), [&](int ia, int ib) {
      return translate_x[ia] < translate_x[ib];
    });

    std::vector<hash::NFPCacheValue*> tmp_nfps;
    tmp_nfps.reserve(size);
    for (int j = 0; j < size; ++j) {
      tmp_nfps.emplace_back(std::move(nfps[I[j]]));
    }
    nfps.swap(tmp_nfps);

    std::vector<geo::Transformation> tmp_translations;
    tmp_translations.reserve(size);
    for (int j = 0; j < size; ++j) {
      tmp_translations.emplace_back(std::move(translations[I[j]]));
    }
    nfps.swap(tmp_nfps);
  }
  for (int j = 0; j < size; ++j) {
    translated_nfps.push_back(
        geo::transform_polygon_with_holes(translations[j], nfps[j]->nfp));
  }
}
std::vector<geo::Point_2> CandidatePoints::get_perfect_points() {
  if (has_boundary) {
    if (boundary.is_empty())
      return {};
    geo::Polygon_set_2 all_nfps;
    all_nfps.join(translated_nfps.begin(), translated_nfps.end());
    geo::Polygon_set_2 target_region;
    target_region.insert(boundary);
    target_region.difference(all_nfps);
    std::vector<geo::Point_2> points;
    std::vector<geo::Polygon_with_holes_2> result;
    target_region.polygons_with_holes(std::back_inserter(result));
    for (auto& pgn : result) {
      geo::strict_simplify(pgn);
      for (auto v = pgn.outer_boundary().vertices_begin();
           v != pgn.outer_boundary().vertices_end(); v++) {
        if (is_valid(*v)) {
          points.push_back(*v);
        }
      }
      for (auto it = pgn.holes_begin(); it != pgn.holes_end(); it++) {
        const auto& hole = *it;
        points.insert(std::end(points), hole.vertices_begin(),
                      hole.vertices_end());
      }
    }
    target_region.polygons_with_holes(std::back_inserter(result));
    return points;
  }
  return {};
}
std::vector<geo::Point_2> nesting::CandidatePoints::get_arrangement_points() {
  if (has_boundary) {
    if (boundary.is_empty())
      return {};
    double p = rand::small_sigma_left_nd01();
    size_t batch = (std::min)((size_t)(p * size), batch_size);
    std::vector<geo::Point_2> points;
    std::vector<geo::Segment_2> edges;
    std::copy(boundary.edges_begin(), boundary.edges_end(),
              std::back_inserter(edges));
    size_t start = 0;
    size_t end = size;
    if (batch < size) {
      start = rand::random01() * (size - batch);
      end = start + batch;
    }
    // std::clog << size << ", " << batch << std::endl;
    for (size_t i = start; i < end; i++) {
      auto& nfp = translated_nfps[i];
      auto& outer = nfp.outer_boundary();
      std::copy(outer.edges_begin(), outer.edges_end(),
                std::back_inserter(edges));
      auto& holes = nfp.holes();
      auto holes_size = holes.size();
      for (size_t j = 0; j < holes_size; j++) {
        auto& hole = holes[j];
        std::copy(hole.edges_begin(), hole.edges_end(),
                  std::back_inserter(edges));
      }
    }
    // TODO 可以扩大start和end的范围，然后并行求交点
    CGAL::compute_intersection_points(edges.begin(), edges.end(),
                                      std::back_inserter(points), true);
    std::shuffle(points.begin(), points.end(), rand::random_engine1);
    std::vector<geo::Point_2> filtered;
    filtered.reserve(points.size());
    for (auto& pt : points) {
      if (filtered.size() > max_candidate_points) {
        break;
      }
      if (is_valid(pt)) {
        filtered.push_back(pt);
      }
    }
    return filtered;
  }
  return {};
}

}  // namespace nesting
