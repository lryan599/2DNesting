#pragma once
#include "basic.h"
namespace nesting {
struct Sheet {
  geo::FT width;
  geo::FT height;
  geo::Polygon_with_holes_2 sheet;
  // 构造函数
  explicit Sheet(geo::FT _width, geo::FT _height)
      : width(_width), height(_height) {
    auto& outer = sheet.outer_boundary();
    outer.push_back(geo::Point_2(0, 0));
    outer.push_back(geo::Point_2(_width, 0));
    outer.push_back(geo::Point_2(_width, _height));
    outer.push_back(geo::Point_2(0, _height));
  }
  // 将sheet的宽度变为_width
  inline void set_width(geo::FT _width) {
    width = _width;
    auto& outer = sheet.outer_boundary();
    auto p1 = outer.vertices_begin() + 1;
    outer.erase(p1);
    outer.erase(p1);
    outer.insert(p1, geo::Point_2(_width, height));
    outer.insert(p1, geo::Point_2(_width, 0));
  }
  // 将sheet的高度变为_height
  inline void set_height(geo::FT _height) {
    height = _height;
    auto& outer = sheet.outer_boundary();
    auto p1 = outer.vertices_begin() + 2;
    outer.erase(p1);
    outer.erase(p1);
    outer.insert(p1, geo::Point_2(0, _height));
    outer.insert(p1, geo::Point_2(width, _height));
  }
};
}  // namespace nesting