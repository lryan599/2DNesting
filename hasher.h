#pragma once
#include <CGAL/Handle_hash_function.h>
#include <boost/functional/hash.hpp>
#include "basic.h"

namespace nesting {
namespace hash {
// 计算Point的哈希值
struct PointHasher {
  std::size_t operator()(const geo::Point_2& p) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(CGAL::to_double(p.x())));
    boost::hash_combine(seed, boost::hash_value(CGAL::to_double(p.y())));
    return seed;
  }
};
// 计算Polygon_with_holes_2哈希值的哈希计算器
struct PolygonHasher {
  std::size_t operator()(const geo::Polygon_with_holes_2& k) const {
    std::size_t seed = 0;
    for (auto hole = k.holes_begin(); hole != k.holes_end(); hole++) {
      for (auto vertex = hole->vertices_begin(); vertex != hole->vertices_end();
           vertex++) {
        boost::hash_combine(seed,
                            boost::hash_value(CGAL::to_double(vertex->x())));
        boost::hash_combine(seed,
                            boost::hash_value(CGAL::to_double(vertex->y())));
      }
    }
    for (auto vertex = k.outer_boundary().vertices_begin();
         vertex != k.outer_boundary().vertices_end(); vertex++) {
      boost::hash_combine(seed,
                          boost::hash_value(CGAL::to_double(vertex->x())));
      boost::hash_combine(seed,
                          boost::hash_value(CGAL::to_double(vertex->y())));
    }
    return seed;
  }
};
// 计算NFP的缓存，由polyA，polyB及其对应的旋转角度rotation_A，rotationB唯一确定。
struct NFPCacheKey {
  explicit NFPCacheKey(const geo::Polygon_with_holes_2* _poly_A,
                       const geo::Polygon_with_holes_2* _poly_B,
                       const uint32_t _rotation_A,
                       const uint32_t _rotation_B)
      : poly_A(_poly_A),
        poly_B(_poly_B),
        rotation_A(_rotation_A),
        rotation_B(_rotation_B) {}
  bool operator==(const NFPCacheKey& rhs) const {
    return (poly_A == rhs.poly_A) && (poly_B == rhs.poly_B) &&
           (rotation_A == rhs.rotation_A) && (rotation_B == rhs.rotation_B);
  }
  const geo::Polygon_with_holes_2* poly_A;
  const geo::Polygon_with_holes_2* poly_B;
  uint32_t rotation_A;
  uint32_t rotation_B;
};
// 用于计算NFPCacheKey哈希值的哈希计算器
struct NFPCacheKeyHasher {
  std::size_t operator()(const NFPCacheKey& k) const {
    // Start with a hash value of 0    .
    std::size_t seed = 0;
    auto hasher = CGAL::Handle_hash_function();
    boost::hash_combine(seed, hasher(k.poly_A));
    boost::hash_combine(seed, hasher(k.poly_B));
    boost::hash_combine(seed, boost::hash_value(k.rotation_A));
    boost::hash_combine(seed, boost::hash_value(k.rotation_B));
    return seed;
  }
};
// nfp及其xy坐标的最值
struct NFPCacheValue {
  geo::Polygon_with_holes_2 nfp;
  double xmin = 0;
  double xmax = 0;
  double ymin = 0;
  double ymax = 0;
};

// 计算PD的缓存键值
struct PDCacheKey {
  explicit PDCacheKey(const geo::Polygon_with_holes_2* _nfp,
                      const double _x,
                      const double _y)
      : nfp(_nfp), x(_x), y(_y) {}
  bool operator==(const PDCacheKey& pdk) const {
    return (nfp == pdk.nfp) && (std::abs(x - pdk.x) < geo::BIAS) &&
           (std::abs(y - pdk.y) < geo::BIAS);
  }
  // nfp一定得是nfp_cache中的nfp指针
  const geo::Polygon_with_holes_2* nfp;
  double x;
  double y;
};

// 用于计算PDCacheKey哈希值的哈希计算器
struct PDCacheKeyHasher {
  std::size_t operator()(const PDCacheKey& k) const {
    // Start with a hash value of 0
    std::size_t seed = 0;
    auto hasher = CGAL::Handle_hash_function();
    boost::hash_combine(seed, hasher(k.nfp));
    boost::hash_combine(seed, boost::hash_value(k.x));
    boost::hash_combine(seed, boost::hash_value(k.y));
    return seed;
  }
};
}  // namespace hash
}  // namespace nesting