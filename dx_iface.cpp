/******************************************************************************
**  dwg2dxf - Program to convert dwg/dxf to dxf(ascii & binary)              **
**                                                                           **
**  Copyright (C) 2015 José F. Soriano, rallazz@gmail.com                    **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#include "dx_iface.h"
#include <algorithm>
#include <iostream>
#include "libdwgr.h"
#include "libdxfrw.h"

bool dx_iface::fileImport(const std::string& fileI, bool debug) {
  unsigned int found = fileI.find_last_of(".");
  std::string fileExt = fileI.substr(found + 1);
  std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::toupper);
  // cData = fData;
  // currentBlock = cData->mBlock;

  if (fileExt == "DXF") {
    // loads dxf
    std::cout << "input: " << fileI << std::endl;
    dxfRW* dxf = new dxfRW(fileI.c_str());
    if (debug) {
      dxf->setDebug(DRW::DebugLevel::Debug);
    }
    bool success = dxf->read(this, true);
    if (!success) {
      std::cout << "DXF file error: format " << dxf->getVersion() << " error "
                << dxf->getError() << std::endl;
    }
    delete dxf;
    return success;
  } else if (fileExt == "DWG") {
    // loads dwg
    dwgR* dwg = new dwgR(fileI.c_str());
    if (debug) {
      dwg->setDebug(DRW::DebugLevel::Debug);
    }
    bool success = dwg->read(this, true);
    if (!success) {
      std::cout << "DWG file error: format " << dwg->getVersion() << " error "
                << dwg->getError() << std::endl;
    }
    delete dwg;
    return success;
  }
  std::cout << "file extension only can be dxf or dwg" << std::endl;
  return false;
}

bool dx_iface::fileExport(const std::string& file,
                          DRW::Version v,
                          bool binary,
                          bool debug) {
  dxfW = new dxfRW(file.c_str());
  if (debug) {
    dxfW->setDebug(DRW::DebugLevel::Debug);
  }
  bool success = dxfW->write(this, v, binary);
  delete dxfW;
  return success;
}

void dx_iface::writeEntity(DRW_Entity* e) {
  switch (e->eType) {
    case DRW::POINT:
      dxfW->writePoint(static_cast<DRW_Point*>(e));
      break;
    case DRW::LINE:
      dxfW->writeLine(static_cast<DRW_Line*>(e));
      break;
    case DRW::CIRCLE:
      dxfW->writeCircle(static_cast<DRW_Circle*>(e));
      break;
    case DRW::ARC:
      dxfW->writeArc(static_cast<DRW_Arc*>(e));
      break;
    case DRW::SOLID:
      dxfW->writeSolid(static_cast<DRW_Solid*>(e));
      break;
    case DRW::ELLIPSE:
      dxfW->writeEllipse(static_cast<DRW_Ellipse*>(e));
      break;
    case DRW::LWPOLYLINE:
      dxfW->writeLWPolyline(static_cast<DRW_LWPolyline*>(e));
      break;
    case DRW::POLYLINE:
      dxfW->writePolyline(static_cast<DRW_Polyline*>(e));
      break;
    case DRW::SPLINE:
      dxfW->writeSpline(static_cast<DRW_Spline*>(e));
      break;
      //    case RS2::EntitySplinePoints:
      //        writeSplinePoints(static_cast<DRW_Point*>(e));
      //        break;
      //    case RS2::EntityVertex:
      //        break;
    case DRW::INSERT:
      dxfW->writeInsert(static_cast<DRW_Insert*>(e));
      break;
    case DRW::MTEXT:
      dxfW->writeMText(static_cast<DRW_MText*>(e));
      break;
    case DRW::TEXT:
      dxfW->writeText(static_cast<DRW_Text*>(e));
      break;
    case DRW::DIMLINEAR:
    case DRW::DIMALIGNED:
    case DRW::DIMANGULAR:
    case DRW::DIMANGULAR3P:
    case DRW::DIMRADIAL:
    case DRW::DIMDIAMETRIC:
    case DRW::DIMORDINATE:
      dxfW->writeDimension(static_cast<DRW_Dimension*>(e));
      break;
    case DRW::LEADER:
      dxfW->writeLeader(static_cast<DRW_Leader*>(e));
      break;
    case DRW::HATCH:
      dxfW->writeHatch(static_cast<DRW_Hatch*>(e));
      break;
    case DRW::IMAGE:
      dxfW->writeImage(static_cast<DRW_Image*>(e),
                       static_cast<dx_ifaceImg*>(e)->path);
      break;
    default:
      break;
  }
}

void dx_iface::addHeader(const DRW_Header* data) {
  qDebug() << "add Header: ";
}

void dx_iface::addLType(const DRW_LType& data) {
  qDebug() << "add LType: ";
}

void dx_iface::addLayer(const DRW_Layer& data) {
  qDebug() << "add Layer: ";
}

void dx_iface::addDimStyle(const DRW_Dimstyle& data) {
  qDebug() << "add DimStyle: ";
}

void dx_iface::addVport(const DRW_Vport& data) {
  qDebug() << "add Vport: ";
}

void dx_iface::addTextStyle(const DRW_Textstyle& data) {
  qDebug() << "add TextStyle: ";
}

void dx_iface::addAppId(const DRW_AppId& data) {
  qDebug() << "add AppId: ";
}

void dx_iface::addLine(const DRW_Line& data) {
  qDebug() << "add Line: ";
  qDebug() << "(" << data.basePoint.x << ", " << data.basePoint.y << ")"
           << ", "
           << "(" << data.secPoint.x << ", " << data.secPoint.y << ")";
  // 假设读入的边是按顺序排列的
  pwh->outer_boundary().push_back(
      nesting::geo::Point_2(data.basePoint.x, data.basePoint.y));
}

void dx_iface::addLWPolyline(const DRW_LWPolyline& data) {
  qDebug() << "add LWPolyline: ";
  qDebug() << "flags: " << data.flags;
  qDebug() << "vertices num: " << data.vertexnum;
  if (data.flags == 1) {
    for (auto& s : data.vertlist) {
      pwh->outer_boundary().push_back(
          nesting::geo::Point_2(s.get()->x, s.get()->y));
    }
  } else if (data.flags == 0) {
    if (data.vertexnum <= 3) {
      throw std::runtime_error("Error addLWPolyline(): vertexnum <= 3");
    } else if (data.vertlist[0] != data.vertlist[data.vertexnum - 1]) {
      throw std::runtime_error("Error addLWPolyline(): part is not closed");
    }
    for (int i = 0; i < data.vertexnum - 1; i++) {
      auto& s = data.vertlist[i];
      pwh->outer_boundary().push_back(
          nesting::geo::Point_2(s.get()->x, s.get()->y));
    }
  }
}

void dx_iface::writeHeader(DRW_Header& data) {
  // complete copy of header vars:
  // data = cData->headerC;
  // or copy one by one:
  //        for (auto it=cData->headerC.vars.begin(); it !=
  //        cData->headerC.vars.end(); ++it)
  //            data.vars[it->first] = new DRW_Variant( *(it->second) );
}

void dx_iface::writeEntities() {
  // for (std::list<DRW_Entity*>::const_iterator it =
  // cData->mBlock->ent.begin();
  //      it != cData->mBlock->ent.end(); ++it)
  //   writeEntity(*it);
  for (auto& p : *solutions) {
    auto& outer = p.outer_boundary();
    DRW_LWPolyline pwl;
    pwl.flags = 1;
    for (auto& v : outer) {
      pwl.addVertex(DRW_Vertex2D(CGAL::to_double(v.x()), CGAL::to_double(v.y()), 0));
    }
    writeEntity(&pwl);
  }
}
