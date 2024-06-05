#ifndef SVGHELPER_H
#define SVGHELPER_H

#include <QDebug>
#include <QDomDocument>
#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>
#include <QVector>
#include <QXmlStreamReader>
#include <QtXml>
#include <vector>

using namespace std;

const QList<QChar> cmdList = {'M', 'm', 'L', 'l', 'H', 'h', 'V', 'v', 'C', 'c',
                              'S', 's', 'Q', 'q', 'T', 't', 'A', 'a', 'Z', 'z'};
const QList<QChar> cmdTypeList = {'M', 'L', 'H', 'V', 'C',
                                  'S', 'Q', 'T', 'A', 'Z'};
const QList<QChar> splitList = {' ', '-', ',', '\r', '\n', '\t'};
const QList<QString> unitList = {"em", "px", "%", "cm", "mm", "in", "pt", "pc"};
const QList<QString> typeList = {"path", "rect",    "circle",  "ellipse",
                                 "line", "polygon", "polyline"};

#define ABSOLUTE_COORDINATES 1
#define RELATIVE_COORDINATES 2

class SvgHelper {
 public:
  SvgHelper();

  void parseSvgImage(QString filepath);
  QList<QPainterPath> getSvgPathList() const;

  QList<QList<QPointF>> getSvgPointList() const;

 private:
  void parseSVGTag(QDomElement e, QString tagname);
  void parseSvgPath(QString path, QPainterPath& paintPath);
  void dealParsePainter(QPainterPath& path, QString line);
  QVector<double> segmentationCoordinates(QString value);
  double getValueWithoutUnit(QString input);
  double radian(double ux, double uy, double vx, double vy);
  int svgArcToCenterParam(double x1,
                          double y1,
                          double rx,
                          double ry,
                          double phi,
                          double fA,
                          double fS,
                          double x2,
                          double y2,
                          double& cx_out,
                          double& cy_out,
                          double& startAngle_out,
                          double& deltaAngle_out);
  double get_angle_with_points(double x1,
                               double y1,
                               double x2,
                               double y2,
                               double x3,
                               double y3);

  QString filepath;
  QPainterPath paintPath;
  QList<QPointF> testpathlist;
  QPointF nowPositon = QPointF(0, 0);
  QPointF pathStartPosition = QPointF(0, 0);
  QPointF lastControlPosition = QPointF(0, 0);
  QList<QPainterPath> svgPathList;
  QList<QList<QPointF>> svgPointList;
};

#endif  // SVGHELPER_H