#pragma once
#include <QSvgRenderer>
#include "csv.h"
#include "dx_data.h"
#include "dx_iface.h"
#include "part.h"
#include "sheet.h"
#include "transformed_shape.h"
#include "svg_helper.h"
namespace nesting {
void read_from_csv(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity);

void read_from_cad(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity);

void read_from_svg(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity);

bool write_to_txt(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res);

void polygon_to_svg(std::ofstream& ofs, const Polygon_with_holes_2& p);

bool write_to_svg(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res);

bool write_to_dxf(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res);

}  // namespace nesting
