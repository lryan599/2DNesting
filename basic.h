#pragma once

#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_set_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/draw_polygon_2.h>
#include <CGAL/draw_polygon_with_holes_2.h>

namespace nesting {
namespace geo {

constexpr double BIAS = 1.1920928955078125e-7L;    // 容忍误差，2^(-23)
constexpr double INF = 0x7FFFFFFFFFFFFFFFL / 1.0;  // 无穷，2^(64)-1
constexpr double PI = 3.14159265358979323846264338327950288L;  // 圆周率PI
using K = CGAL::
    Exact_predicates_exact_constructions_kernel;  // 精确的几何断言和精确的几何构造内核
using Traits_2 = CGAL::Gps_segment_traits_2<K>;  // 默认的Traits，这里显式使用
using Point_2 = Traits_2::Point_2;               // 二维坐标点
using Vector_2 = Traits_2::Vector_2;             // 二维向量
using Segment_2 = Traits_2::Segment_2;           // 二维线段
using Polygon_2 = Traits_2::Polygon_2;  // 二维多边形，由顶点组成
// 带孔的二维多边形。其外部是相对简单的多边形，内部的空洞是简单多边形。
using Polygon_with_holes_2 = Traits_2::Polygon_with_holes_2;
// 多边形集合操作容器，可以进行取交、取并等操作
using Polygon_set_2 = CGAL::Polygon_set_2<K>;
namespace Simplification = CGAL::Polyline_simplification_2;  // 多边形简化算法
using FT = K::FT;  // 内核所用的数据类型,CGAL根据所用库进行选择
using Transformation =
    CGAL::Aff_transformation_2<K>;  // 仿射变换函数，如放缩、平移、旋转等
const Traits_2 traits;              // 一个Traits实例
const FT FT_ZERO(0);                // FTZERO实例
const Transformation rotate_90(CGAL::ROTATION, 1, 0);    // 逆时针旋转90度
const Transformation rotate_180(CGAL::ROTATION, 0, -1);  // 逆时针旋转180度
const Transformation rotate_270(CGAL::ROTATION, -1, 0);  // 逆时针旋转270度
}  // namespace geo
}  // namespace nesting