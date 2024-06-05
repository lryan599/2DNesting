#pragma once
#include <random>

namespace nesting {
namespace rand {
// 随机数引擎
inline std::random_device rd;
inline std::mt19937 random_engine1(rd());
inline std::mt19937 random_engine2(rd());
inline std::mt19937 random_engine3(rd());
inline std::normal_distribution<double> normal_distribution(0, 1);
// 峰值在右，倾向于靠右放置似乎有更强的优化能力
inline std::normal_distribution<double> nd1(0.8, 0.07);
inline std::normal_distribution<double> nd2(0.5, 0.2);
inline std::normal_distribution<double> nd3(0.45, 0.14);
inline std::normal_distribution<double> nd4(0.25, 0.02);
inline std::uniform_real_distribution<> ud1(0.0, 1.0);

inline double random01() {
  return ud1(random_engine1);
}

inline double right_nd01() {
  auto p = nd1(random_engine1);
  if (p < 0) {
    p = 0;
  } else if (p > 1) {
    p = 1;
  }
  return p;
}

inline double center_nd01() {
  auto p = nd2(random_engine2);
  if (p < 0) {
    p = 0;
  } else if (p > 1) {
    p = 1;
  }
  return p;
}

inline double left_nd01() {
  auto p = nd3(random_engine2);
  if (p < 0) {
    p = 0;
  } else if (p > 1) {
    p = 1;
  }
  return p;
}

inline double small_sigma_left_nd01() {
  auto p = nd4(random_engine3);
  if (p < 0) {
    p = 0;
  } else if (p > 1) {
    p = 1;
  }
  return p;
}
}  // namespace rand
}  // namespace nesting