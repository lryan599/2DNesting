#pragma once
#include <CGAL/Boolean_set_operations_2.h>
#include "algorithm.h"
#include "part.h"
#include "sheet.h"
#include "simplification.h"
namespace nesting {
namespace geo {

void preprocess(std::vector<nesting::Item>& items,
                double offset,
                bool need_simplify);

void preprocess(Sheet& sheet,
                double top_offset,
                double left_offset,
                double bottom_offset,
                double right_offset);
}  // namespace geo
}  // namespace nesting
