#pragma once
#include "algorithm.h"

namespace nesting {
typedef geo::Polygon_with_holes_2 Polygon_with_holes_2;
typedef geo::FT FT;
typedef geo::Vector_2 Vector_2;
typedef geo::Transformation Transformation;
struct TransformedShape {
  TransformedShape(const Polygon_with_holes_2* _base,
                   const FT& _x,
                   const FT& _y,
                   const uint32_t& _rotation,
                   const uint32_t& _allowed_rotations,
                   const bool _simplified,
                   const size_t _item_idx);
  TransformedShape(const Polygon_with_holes_2* _base,
                   const double& _x,
                   const double& _y,
                   const uint32_t& _rotation,
                   const uint32_t& _allowed_rotations,
                   const bool _simplified,
                   const size_t _item_idx);
  TransformedShape(const TransformedShape& other);
  // 赋值操作符重载
  TransformedShape& operator=(const TransformedShape& other) {
    // 检查自赋值
    if (this != &other) {
      // const 成员变量不能赋值，因此不需要对其进行操作
      translate_double_x = other.translate_double_x;
      translate_double_y = other.translate_double_y;
      rotation = other.rotation;
      translate_ft_x = other.translate_ft_x;
      translate_ft_y = other.translate_ft_y;
      transformed = other.transformed;
    }
    return *this;
  }
  const Polygon_with_holes_2* base;
  const uint32_t allowed_rotations;
  std::vector<uint32_t> reduced_rotations;
  Polygon_with_holes_2 transformed;
  const bool simplified;
  const size_t item_idx;
  void set_translate(const FT& x, const FT& y) {
    this->translate_ft_x = x;
    this->translate_ft_y = y;
    this->translate_double_x = CGAL::to_double(translate_ft_x);
    this->translate_double_y = CGAL::to_double(translate_ft_y);
    update();
  }
  void set_translate(const double& x, const double& y) {
    this->translate_double_x = x;
    this->translate_double_y = y;
    this->translate_ft_x = x;
    this->translate_ft_y = y;
    update();
  }
  void set_rotation(uint32_t _rotation) {
    this->rotation = _rotation;
    update();
  }
  void set(const uint32_t& _rotation, const FT& x, const FT& y) {
    this->rotation = _rotation;
    this->set_translate(x, y);
  }
  void set(const uint32_t& _rotation, const double& x, const double& y) {
    this->rotation = _rotation;
    this->set_translate(x, y);
  }
  void set(const uint32_t& _rotation,
           const double& x,
           const double& y,
           const Polygon_with_holes_2& p) {
    this->rotation = _rotation;
    this->transformed = p;
    this->translate_double_x = x;
    this->translate_double_y = y;
    this->translate_ft_x = x;
    this->translate_ft_y = y;
  }
  void set(const uint32_t& _rotation,
           const FT& x,
           const FT& y,
           const Polygon_with_holes_2& p) {
    this->rotation = _rotation;
    this->transformed = p;
    this->translate_ft_x = x;
    this->translate_ft_y = y;
    this->translate_double_x = CGAL::to_double(translate_ft_x);
    this->translate_double_y = CGAL::to_double(translate_ft_y);
  }
  std::pair<FT, FT> get_translate_ft() const {
    return std::make_pair(translate_ft_x, translate_ft_y);
  }
  std::pair<double, double> get_translate_double() const {
    return std::make_pair(translate_double_x, translate_double_y);
  }
  FT& get_translate_ft_x() { return translate_ft_x; }
  FT& get_translate_ft_y() { return translate_ft_y; }
  double get_translate_double_x() const { return translate_double_x; }
  double get_translate_double_y() const { return translate_double_y; }
  Transformation get_translate() const {
    return Transformation(CGAL::TRANSLATION,
                          Vector_2(translate_ft_x, translate_ft_y));
  }
  uint32_t get_rotation() const { return this->rotation; }
  void update();
  // 简化旋转范围：如果旋转后的图形和旋转前一样，则去除掉这个旋转角度
  void set_reduced_rotations();

 private:
  uint32_t rotation;
  FT translate_ft_x;
  FT translate_ft_y;
  double translate_double_x;
  double translate_double_y;
};
}  // namespace nesting
