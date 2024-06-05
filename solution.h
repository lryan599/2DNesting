#pragma once
#include "transformed_shape.h"

struct Solution {
  double length;
  double utilization;
  double time;
  std::vector<nesting::TransformedShape> solution;

  Solution(const double& _length,
           const double& _util,
           const double& _time,
           const std::vector<nesting::TransformedShape>& _solution)
      : length(_length), utilization(_util), time(_time), solution(_solution) {}
};