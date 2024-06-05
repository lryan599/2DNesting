#include "transformed_shape.h"

namespace nesting {
TransformedShape::TransformedShape(const geo::Polygon_with_holes_2* _base,
                                   const geo::FT& _x,
                                   const geo::FT& _y,
                                   const uint32_t& _rotation,
                                   const uint32_t& _allowed_rotations,
                                   const bool _simplified,
                                   const size_t _item_idx)
    : base(_base),
      translate_ft_x(_x),
      translate_ft_y(_y),
      rotation(_rotation),
      allowed_rotations(_allowed_rotations),
      simplified(_simplified),
      item_idx(_item_idx) {
  this->translate_double_x = CGAL::to_double(translate_ft_x);
  this->translate_double_y = CGAL::to_double(translate_ft_y);
  set_reduced_rotations();
  update();
}

TransformedShape::TransformedShape(const Polygon_with_holes_2* _base,
                                   const double& _x,
                                   const double& _y,
                                   const uint32_t& _rotation,
                                   const uint32_t& _allowed_rotations,
                                   const bool _simplified,
                                   const size_t _item_idx)
    : base(_base),
      translate_double_x(_x),
      translate_double_y(_y),
      rotation(_rotation),
      allowed_rotations(_allowed_rotations),
      simplified(_simplified),
      item_idx(_item_idx) {
  this->translate_ft_x = translate_double_x;
  this->translate_ft_y = translate_double_y;
  set_reduced_rotations();
  update();
}

TransformedShape::TransformedShape(const TransformedShape& other)
    : base(other.base),
      translate_double_x(other.translate_double_x),
      translate_double_y(other.translate_double_y),
      rotation(other.rotation),
      allowed_rotations(other.allowed_rotations),
      simplified(other.simplified),
      item_idx(other.item_idx),
      translate_ft_x(other.translate_ft_x),
      translate_ft_y(other.translate_ft_y),
      transformed(other.transformed),
      reduced_rotations(other.reduced_rotations) {}

void TransformedShape::update() {
  auto rotate = geo::get_rotate(this->rotation, this->allowed_rotations);
  auto translate = get_translate();
  if (rotate) {
    this->transformed =
        geo::transform_polygon_with_holes(translate * (*rotate), *base);
  } else {
    this->transformed = geo::transform_polygon_with_holes(translate, *base);
  }
}

void TransformedShape::set_reduced_rotations() {
  reduced_rotations.push_back(0);
  for (uint32_t i = 1; i < allowed_rotations; i++) {
    auto rotate = geo::get_rotate(i, allowed_rotations);
    if (rotate) {
      auto rotated = geo::transform_polygon_with_holes(*rotate, *base);
      bool flag = true;
      for (auto& j : reduced_rotations) {
        auto rotate_j = geo::get_rotate(j, allowed_rotations);
        Polygon_with_holes_2 existed;
        if (rotate_j) {
          existed = geo::transform_polygon_with_holes(*rotate_j, *base);
        } else {
          existed = *base;
        }
        if (geo::is_translated(existed, rotated)) {
          flag = false;
          break;
        }
      }
      if (flag) {
        reduced_rotations.push_back(i);
      }
    }
  }
}

}  // namespace nesting
