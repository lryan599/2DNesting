#include "data_converter.h"

namespace nesting {
void read_from_csv(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity) {
  rapidcsv::Document document(path);
  auto pgns = document.GetColumn<std::string>("polygon");
  auto rots = document.GetColumn<uint32_t>("allowed_rotations");
  auto quat = document.GetColumn<uint32_t>("quantity");
  for (auto& s : pgns) {
    polygons.emplace_back();
    std::istringstream(s) >> polygons.back();
  }
  allowed_rotations.swap(rots);
  quantity.swap(quat);
}

void read_from_cad(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity) {
  dx_iface iface;
  polygons.emplace_back();
  iface.pwh = &polygons.back();
  auto file = path.c_str();
  auto ok = iface.fileImport(file, false);
  if (!ok) {
    throw std::runtime_error("Error read_from_cad(): can not import cad file");
  }
  allowed_rotations.push_back(1);
  quantity.push_back(1);
}

void read_from_svg(const std::string& path,
                   std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
                   std::vector<uint32_t>& allowed_rotations,
                   std::vector<uint32_t>& quantity) {
  auto pth = QString::fromStdString(path);
  QSvgRenderer svgRenderer(pth);
  if (!svgRenderer.isValid()) {
    throw std::runtime_error("Error read_from_svg(): invalid svg file");
  }
  SvgHelper svgHelper;
  svgHelper.parseSvgImage(pth);
  auto list = svgHelper.getSvgPointList();
  for (auto& pgn : list) {
    polygons.emplace_back();
    // 最后一个点可能冗余
    if (pgn.first() == pgn.last()) {
      pgn.removeLast();
    }
    auto& outer = polygons.back().outer_boundary();
    for (auto& v : pgn) {
      outer.push_back(nesting::geo::Point_2(v.x(), v.y()));
    }
    allowed_rotations.push_back(1);
    quantity.push_back(1);
  }
  qDebug() << list;
}

bool write_to_txt(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res) {
  // 打开文件
  std::ofstream outputFile(path);

  // 检查文件是否成功打开
  if (!outputFile.is_open()) {
    std::cerr << "Error opening file." << std::endl;
    return false;
  }
  outputFile << std::setiosflags(std::ios::fixed) << std::setprecision(8);
  outputFile << "Best Utilization: " << best_utilization << std::endl;
  outputFile << "Sheet (W, L): "
             << "(" << sheet_width << ", " << sheet_length << ")" << std::endl;
  outputFile << "Placement: " << std::endl;
  for (auto& pwh : res) {
    outputFile << pwh << std::endl;
  }
  // 关闭文件
  outputFile.close();
  return true;
}

void polygon_to_svg(std::ofstream& ofs, const Polygon_with_holes_2& p) {
  ofs << "<polygon fill=\"grey\" stroke=\"black\" points=\"";
  auto& outer = p.outer_boundary();
  for (auto& v : outer) {
    ofs << v.x() << "," << v.y() << " ";
  }
  ofs << "\"/> " << std::endl;
  for (auto& h : p.holes()) {
    ofs << "<polygon fill=\"white\" stroke=\"black\" points=\"";
    for (auto& v : outer) {
      ofs << v.x() << "," << v.y() << " ";
    }
    ofs << "\"/> " << std::endl;
  }
}

bool write_to_svg(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res) {
  // 打开文件
  std::ofstream outputFile(path);

  // 检查文件是否成功打开
  if (!outputFile.is_open()) {
    std::cerr << "Error opening file." << std::endl;
    return false;
  }
  outputFile << std::setiosflags(std::ios::fixed) << std::setprecision(8);
  // svg头部
  outputFile << "<svg width=\"" << sheet_length << "\" height=\"" << sheet_width
             << "\" viewBox=\"" << -0.25 * sheet_length << ", "
             << -0.25 * sheet_width << ", " << 1.5 * sheet_length << ", "
             << 1.5 * sheet_width << "\" xmlns=\"http://www.w3.org/2000/svg\">"
             << std::endl;
  // sheet
  outputFile << "<rect width=\"" << sheet_length << "\" height=\""
             << sheet_width << "\" fill=\"white\" stroke=\"black\"/>"
             << std::endl;
  // polygons
  for (auto& pwh : res) {
    polygon_to_svg(outputFile, pwh);
  }
  // svg尾部
  outputFile << "</svg>" << std::endl;
  // 关闭文件
  outputFile.close();
  return true;
}

bool write_to_dxf(const std::string& path,
                  const double& best_utilization,
                  const double& sheet_width,
                  const double& sheet_length,
                  const std::vector<geo::Polygon_with_holes_2>& res) {
  dx_iface iface;
  iface.solutions = const_cast<std::vector<geo::Polygon_with_holes_2>*>(&res);
  return iface.fileExport(path, DRW::Version::AC1014, false, false);
}

}  // namespace nesting
