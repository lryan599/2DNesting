#pragma once
#include <CGAL/minkowski_sum_2.h>
#include "candidate_points.h"
#include "solution.h"
#include "layout.h"
namespace nesting {

typedef typename geo::FT FT;
typedef typename geo::Polygon_with_holes_2 Polygon_with_holes_2;
typedef typename geo::Transformation Transformation;
typedef typename geo::Point_2 Point_2;
typedef typename hash::NFPCacheKey NFPCacheKey;
typedef typename hash::NFPCacheValue NFPCacheValue;
typedef typename hash::NFPCacheKeyHasher NFPCacheKeyHasher;

/// <summary>
/// 求固定A环绕B得到的临界多边形NFP，A和B均为带孔多边形，NFP以带孔的多边形给出。
/// <para>原理：minkowski之差，NFP=A(+)-B。可应用在带孔多边形上。</para>
/// <para>注意：</para>
/// <para>1. 尝试从layout中读取缓存，若无则写入缓存</para>
/// <para>2. A和B必须是规范的多边形</para>
/// <para>3. 退化成线和点的NFP无法求出。</para>
/// <para>4. 生成的NFP是以B的第一个顶点为参考点，绕A生成的NFP</para>
/// </summary>
/// <param name="poly_A">规范的polygon A</param>
/// <param name="rotation_A">A的旋转变换</param>
/// <param name="allowed_rotation_A">A的可旋转范围</param>
/// <param name="poly_B">规范的polygon B</param>
/// <param name="rotation_B">B的旋转变换</param>
/// <param name="allowed_rotation_B">B的可旋转范围</param>
/// <param name="layout">给定的布局情况</param>
/// <returns>所计算的NFPCacheValue</returns>
NFPCacheValue& comp_nfp(const Polygon_with_holes_2* poly_A,
                        const uint32_t rotation_A,
                        const uint32_t allowed_rotation_A,
                        const Polygon_with_holes_2* poly_B,
                        const uint32_t rotation_B,
                        const uint32_t allowed_rotation_B,
                        Layout& layout);

/// @brief 计算point(px, py)相对v.nfp的pd
/// @param v NFPCacheValue& v
/// @param px point的x坐标
/// @param py point的y坐标
/// @param layout 给定的布局情况
/// @return 所计算的pd
FT comp_pd(NFPCacheValue& v, double px, double py, Layout& layout);

void shrink(Layout& layout);

inline void expand(Layout& layout) {
  layout.cur_length *= (1 + layout.rinc);
  // 更新sheet
  layout.sheets[0].set_width(layout.cur_length);
}

// 一个简单的计算初始解的算法，目前适用于二维带排样
void get_init_solu(Layout& layout);

// TODO 热点函数优化
bool minimize_overlap(Layout& layout, volatile bool* requestQuit);

inline void print_result(const Layout& layout, const clock_t& start) {
  std::clog << "========================Solution Start========================"
            << std::endl;
  std::clog << "time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "s"
            << std::endl;
  std::clog << "length: " << layout.cur_length << std::endl;
  std::clog << "utilization rate: " << layout.best_utilization << std::endl;
  for (auto& p : layout.sheet_parts[0]) {
    auto& x = p.transformed;
    std::clog << x << std::endl;
  }
  std::clog << "=========================Solution End========================="
            << std::endl;
}

void GOMH(Layout& layout,
          size_t max_time,
          std::function<void(const Solution&)> ProgressHandler,
          volatile bool* requestQuit);
}  // namespace nesting