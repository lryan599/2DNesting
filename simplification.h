#pragma once
#ifndef SIMPLIFICATION_COST_H
#define SIMPLIFICATION_COST_H

#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/enum.h>

#include <boost/optional/optional.hpp>
#include "basic.h"

namespace CGAL {

#ifndef DOXYGEN_RUNNING

template <class Tr>
class Constrained_triangulation_plus_2;

#endif

namespace Polyline_simplification_2 {
class Convex_squared_distance_cost {
 public:
  Convex_squared_distance_cost() {}

  template <class CDT>
  boost::optional<typename CDT::Geom_traits::FT> operator()(
      const Constrained_triangulation_plus_2<CDT>& pct,
      typename Constrained_triangulation_plus_2<
          CDT>::Vertices_in_constraint_iterator vicq) const {
    typedef typename Constrained_triangulation_plus_2<
        CDT>::Points_in_constraint_iterator Points_in_constraint_iterator;
    typedef
        typename Constrained_triangulation_plus_2<CDT>::Geom_traits Geom_traits;
    typedef typename Geom_traits::FT FT;
    typedef typename Geom_traits::Compute_squared_distance_2
        Compute_squared_distance;
    typedef typename Geom_traits::Construct_segment_2 Construct_segment;
    typedef typename Geom_traits::Segment_2 Segment;
    typedef typename Geom_traits::Point_2 Point;

    Compute_squared_distance compute_squared_distance =
        pct.geom_traits().compute_squared_distance_2_object();
    Construct_segment construct_segment =
        pct.geom_traits().construct_segment_2_object();
    typedef typename Constrained_triangulation_plus_2<
        CDT>::Vertices_in_constraint_iterator Vertices_in_constraint_iterator;

    Vertices_in_constraint_iterator vicp = boost::prior(vicq);
    Vertices_in_constraint_iterator vicr = boost::next(vicq);

    Point const& lP = (*vicp)->point();
    Point const& lR = (*vicr)->point();

    Segment lP_R = construct_segment(lP, lR);

    FT d1 = 0.0;
    Points_in_constraint_iterator pp(vicp), rr(vicr);
    ++pp;

    for (; pp != rr; ++pp) {
      auto o = CGAL::orientation(lP, lR, *pp);
      // 凸部分不能被简化
      if (o == CGAL::RIGHT_TURN) {
        d1 = ::nesting::geo::INF;
        break;
      } else {
        d1 = (std::max)(d1, compute_squared_distance(lP_R, *pp));
      }
    }
    return d1;
  }
};
}  // namespace Polyline_simplification_2
}  // namespace CGAL
#endif