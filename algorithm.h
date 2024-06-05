#pragma once
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/Boolean_set_operations_2/intersection.h>
#include "basic.h"
#include "simplification.h"
namespace nesting {
namespace geo {
/// @brief 求p的面积，值为外轮廓面积减去所有洞的面积之和
/// @param p 带孔多边形p
/// @return p的面积
FT pwh_area(const Polygon_with_holes_2& p);

/// @brief 找到p中，纵坐标最小且横坐标最小的顶点
/// @param p 多边形p
Point_2 find_bottom_left_vertex(const Polygon_2& p);

/// @brief 严格地简化多边形，消除三点共线的中点
/// @param p 需要简化的多边形
void simplify_polygon(Polygon_2& p);

/// @brief 严格地简化多边形，消除三点共线的中点
/// @param p 需要简化的多边形
void strict_simplify(Polygon_with_holes_2& p);

/// @brief
/// 判断多边形A是否可由多边形B平移得到，使用该函数先确保B由A旋转得到。
/// @note 可能由于精度误差导致判断错误
/// @param A 多边形A
/// @param B 多边形B
/// @return A是否可由B平移得到
bool is_translated(const Polygon_2& A, const Polygon_2& B);

/// @brief
/// 判断多边形A是否可由多边形B平移得到，使用该函数先确保B由A旋转得到。
/// @note 可能由于精度误差导致判断错误
/// @param A 多边形A
/// @param B 多边形B
/// @return A是否可由B平移得到
bool is_translated(const Polygon_with_holes_2& A,
                   const Polygon_with_holes_2& B);

/// @brief 根据rotation和allowed_rotations返回对应的变形rotate
/// @param rotation 变换的角度
/// @param allowed_rotations 允许的所有角度
/// @return 变形rotate
inline const Transformation* get_rotate(uint32_t rotation,
                                        uint32_t allowed_rotations) {
  if (allowed_rotations == 2) {
    if (rotation == 1) {
      return &rotate_180;
    } else {
      return 0;
    }
  } else if (allowed_rotations == 4) {
    if (rotation == 1) {
      return &rotate_90;
    } else if (rotation == 2) {
      return &rotate_180;
    } else if (rotation == 3) {
      return &rotate_270;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/// @brief 对带孔多边形做变形
/// @param transformation 变形
/// @param pgn 带孔多边形
/// @return 变形后的带孔多边形
Polygon_with_holes_2 transform_polygon_with_holes(
    const Transformation transformation,
    const Polygon_with_holes_2& pgn);

/// @brief 判断pgn是否合法
/// @param pgn 多边形
/// @return pgn是否合法
bool is_valid_polygon(const Polygon_2& pgn);

/// @brief 判断pwh是否合法
/// @param pwh 带孔多边形
/// @return pwh是否合法
bool is_valid_polygon_with_holes(const Polygon_with_holes_2& pwh);

// ------------------------------------------------------------------
//                      PD部分，计算渗透距离
// ------------------------------------------------------------------

/// @brief 若点p在nfp内，求解p移到nfp边界上的最小距离pd
/// @param nfp 给定的nfp
/// @param p 给定的p
/// @return 计算好的pd
FT comp_pd(const Polygon_with_holes_2& nfp, const Point_2& p);

// ------------------------------------------------------------------
//                              计算ifr
// ------------------------------------------------------------------

/// @brief
/// 求固定A，B在A内部环绕得到的内临界多边形IFR(Inner-FitRectangle)，
/// 注意：
/// 1. IFR是粗略给出的，首先将A扩充成包络矩形后，再求解IFR。
/// 2. IFR生成的参考点是B的第一个顶点
/// @param poly_A 多变形A
/// @param poly_B 多边形B
/// @return 计算好的IFR
Polygon_2 comp_ifr(const Polygon_2 poly_A, const Polygon_2 poly_B);

/// @brief
/// 求固定A，B在A内部环绕得到的内临界多边形IFR(Inner-FitRectangle)，
/// @attention
/// 1. IFR是粗略给出的，首先将A扩充成包络矩形后，再求解IFR。
/// 2. IFR生成的参考点是B的第一个顶点
/// @param poly_A 多变形A
/// @param poly_B 多边形B
/// @return 计算好的IFR
Polygon_2 comp_ifr(const Polygon_with_holes_2 poly_A,
                   const Polygon_with_holes_2 poly_B);

// ------------------------------------------------------------------
//                              前处理
// ------------------------------------------------------------------
/// @brief 多边形偏移，主要用于多边形膨胀和收缩
/// @param p 需要偏移的多边形
/// @param offset 偏移量，正值膨胀负值收缩
void offset_polygon(Polygon_with_holes_2& p, double offset);

/// @brief 利用多边形简化算法简化多边形：删除那些偏移较小的凹点
/// @param p 需要简化的多边形
/// @return 返回是否被简化
bool simplify(Polygon_2& p);

/// @brief 利用多边形简化算法简化多边形：删除那些偏移较小的凹点
/// @param p 需要简化的多边形
/// @return 返回是否被简化
bool simplify(Polygon_with_holes_2& p);
}  // namespace geo
}  // namespace nesting