#include "svg_helper.h"

// 方便调试时的打印输出
#define qout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]\t"

SvgHelper::SvgHelper() {}

void SvgHelper::parseSvgImage(QString filepath) {
  this->filepath = filepath;

  QFile svgFile(filepath);
  if (svgFile.open(QFile::ReadOnly)) {
    QDomDocument doc;
    if (doc.setContent(&svgFile)) {
      svgFile.close();
      // 返回根节点
      QDomElement root = doc.documentElement();
      qout << "root:" << root.nodeName();
      // 获得第一个子节点
      QDomNode node = root.firstChild();
      while (!node.isNull())  // 如果节点不空
      {
        if (node.isElement())  // 如果节点是元素
        {
          // 转换为元素
          QDomElement e = node.toElement();
          QString tagname = e.tagName();
          qout << "tagname:" << tagname;
          if (typeList.contains(tagname)) {
            parseSVGTag(e, tagname);
          } else {
            foreach (QString type, typeList) {
              QDomNodeList list = e.elementsByTagName(type);
              if (list.length() > 0) {
                for (int i = 0; i < list.count(); i++) {
                  QDomNode n = list.at(i);
                  parseSVGTag(n.toElement(), n.nodeName());
                }
              }
            }
          }
        }
        // 下一个兄弟节点
        node = node.nextSibling();
      }
    }
  }
}

void SvgHelper::parseSVGTag(QDomElement e, QString tagname) {
  if (QString::compare(tagname, "path", Qt::CaseInsensitive) == 0) {  // 路径
    QString pathvalue = e.attribute("d");
    qout << pathvalue;
    parseSvgPath(pathvalue, paintPath);
  } else if (QString::compare(tagname, "rect", Qt::CaseInsensitive) == 0) {
    double x = getValueWithoutUnit(e.attribute("x"));
    double y = getValueWithoutUnit(e.attribute("y"));
    double width = getValueWithoutUnit(e.attribute("width"));
    double height = getValueWithoutUnit(e.attribute("height"));
    double rx = getValueWithoutUnit(e.attribute("rx"));
    double ry = getValueWithoutUnit(e.attribute("rx"));

    qout << x << y << width << height << rx << ry;

    if (rx < 0 || ry < 0) {
      paintPath.moveTo(x, y);
      paintPath.lineTo(x + width, y);
      paintPath.lineTo(x + width, y + height);
      paintPath.lineTo(x, y + height);
      paintPath.lineTo(x, y);
      paintPath.moveTo(x, y);
      {
        QList<QPointF> list;
        list.append(QPointF(x, y));
        list.append(QPointF(x + width, y));
        list.append(QPointF(x + width, y + height));
        list.append(QPointF(x, y));
        svgPointList.append(list);
      }
    } else {
      double r = qMax(rx, ry);
      paintPath.moveTo(x + r, y);
      paintPath.lineTo(x + width - r, y);
      QRectF rect(x + width - 2 * r, y, 2 * r, 2 * r);
      paintPath.arcTo(rect, 90, -90);
      paintPath.lineTo(x + width, y + height - r);
      rect = QRectF(x + width - 2 * r, y + height - 2 * r, 2 * r, 2 * r);
      paintPath.arcTo(rect, 0, -90);
      paintPath.lineTo(x + r, y + height);
      rect = QRectF(x, y + height - 2 * r, 2 * r, 2 * r);
      paintPath.arcTo(rect, -90, -90);
      paintPath.lineTo(x, y + r);
      rect = QRectF(x, y, 2 * r, 2 * r);
      paintPath.arcTo(rect, 180, -90);
      paintPath.moveTo(x + r, y);
      {
        QPainterPath path;
        QList<QPointF> list;
        list.append(QPointF(x + r, y));
        list.append(QPointF(x + width - r, y));

        QRectF rect(x + width - 2 * r, y, 2 * r, 2 * r);
        path.moveTo(x + width - r, y);
        path.arcTo(rect, 90, -90);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));

        list.append(QPointF(x + width, y + height - r));

        path.clear();
        rect = QRectF(x + width - 2 * r, y + height - 2 * r, 2 * r, 2 * r);
        path.arcTo(rect, 0, -90);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));

        list.append(QPointF(x + r, y + height));

        path.clear();
        rect = QRectF(x, y + height - 2 * r, 2 * r, 2 * r);
        path.arcTo(rect, -90, -90);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));

        list.append(QPointF(x, y + r));

        path.clear();
        rect = QRectF(x, y, 2 * r, 2 * r);
        path.arcTo(rect, 180, -90);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));

        list.append(QPointF(x + r, y));

        svgPointList.append(list);
      }
    }
  } else if (QString::compare(tagname, "circle", Qt::CaseInsensitive) == 0) {
    double cx = getValueWithoutUnit(e.attribute("cx"));
    double cy = getValueWithoutUnit(e.attribute("cy"));
    double r = getValueWithoutUnit(e.attribute("r"));

    cx = cx < 0 ? 0 : cx;
    cy = cy < 0 ? 0 : cy;

    qout << cx << cy << r;

    if (r > 0) {
      paintPath.moveTo(cx + r, cy);
      paintPath.arcTo(cx - r, cy - r, 2 * r, 2 * r, 0, 360);
      paintPath.moveTo(cx + r, cy);
      {
        QPainterPath path;
        QList<QPointF> list;

        path.moveTo(cx + r, cy);
        path.arcTo(cx - r, cy - r, 2 * r, 2 * r, 0, 360);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));
        svgPointList.append(list);
      }
    }
  } else if (QString::compare(tagname, "ellipse", Qt::CaseInsensitive) == 0) {
    double cx = getValueWithoutUnit(e.attribute("cx"));
    double cy = getValueWithoutUnit(e.attribute("cy"));
    double rx = getValueWithoutUnit(e.attribute("rx"));
    double ry = getValueWithoutUnit(e.attribute("ry"));

    cx = cx < 0 ? 0 : cx;
    cy = cy < 0 ? 0 : cy;

    qout << cx << cy << rx << ry;

    if (rx > 0 && ry > 0) {
      paintPath.moveTo(cx + rx, cy);
      paintPath.arcTo(cx - rx, cy - ry, 2 * rx, 2 * ry, 0, 360);
      paintPath.moveTo(cx + rx, cy);
      {
        QPainterPath path;
        QList<QPointF> list;

        path.moveTo(cx + rx, cy);
        path.arcTo(cx - rx, cy - ry, 2 * rx, 2 * ry, 0, 360);
        for (double i = 0; i < 1; i += 1.0 / path.length())
          list.append(path.pointAtPercent(i));
        list.append(path.pointAtPercent(1));
        svgPointList.append(list);
      }
    }
  } else if (QString::compare(tagname, "line", Qt::CaseInsensitive) == 0) {
    double x1 = getValueWithoutUnit(e.attribute("x1"));
    double y1 = getValueWithoutUnit(e.attribute("y1"));
    double x2 = getValueWithoutUnit(e.attribute("x2"));
    double y2 = getValueWithoutUnit(e.attribute("y2"));

    qout << x1 << y1 << x2 << y2;

    paintPath.moveTo(x1, y1);
    paintPath.lineTo(x2, y2);
    paintPath.moveTo(x2, y2);
    {
      QList<QPointF> list;
      list.append(QPointF(x1, y1));
      list.append(QPointF(x2, y2));
      svgPointList.append(list);
    }
  } else if (QString::compare(tagname, "polygon", Qt::CaseInsensitive) == 0) {
    QString value = e.attribute("points");
    qout << value;
    QVector<double> vPos = segmentationCoordinates(value);
    qout << vPos;

    QPointF startPoint;
    QList<QPointF> list;
    if (vPos.length() > 1) {
      startPoint = QPointF(vPos[0], vPos[1]);
      paintPath.moveTo(startPoint);
      list.append(startPoint);
      vPos.removeFirst();
      vPos.removeFirst();
    }

    while (vPos.length() / 2 >= 1) {
      paintPath.lineTo(vPos[0], vPos[1]);
      list.append(QPointF(vPos[0], vPos[1]));
      vPos.removeFirst();
      vPos.removeFirst();
    }

    if (!startPoint.isNull()) {
      paintPath.lineTo(startPoint);
      paintPath.moveTo(startPoint);
      list.append(startPoint);
    }
    if (!list.isEmpty())
      svgPointList.append(list);

  } else if (QString::compare(tagname, "polyline", Qt::CaseInsensitive) == 0) {
    QString value = e.attribute("points");
    qout << value;
    QVector<double> vPos = segmentationCoordinates(value);
    qout << vPos;

    QPointF startPoint;
    QList<QPointF> list;
    if (vPos.length() > 1) {
      startPoint = QPointF(vPos[0], vPos[1]);
      paintPath.moveTo(startPoint);
      list.append(startPoint);
      vPos.removeFirst();
      vPos.removeFirst();
    }

    while (vPos.length() / 2 >= 1) {
      paintPath.lineTo(vPos[0], vPos[1]);
      list.append(QPointF(vPos[0], vPos[1]));
      vPos.removeFirst();
      vPos.removeFirst();
    }

    if (!startPoint.isNull()) {
      paintPath.moveTo(startPoint);
      //            list.append(startPoint);
    }
    if (!list.isEmpty())
      svgPointList.append(list);
  }

  if (!paintPath.isEmpty()) {
    svgPathList.append(paintPath);
  }
  paintPath.clear();
  if (!testpathlist.isEmpty()) {
    svgPointList.append(testpathlist);
  }
  testpathlist.clear();
}

void SvgHelper::parseSvgPath(QString path, QPainterPath& paintPath) {
  QString cmdLine = "";
  foreach (QChar c, path) {
    if (cmdList.contains(c)) {
      qout << cmdLine;
      if (!cmdLine.isEmpty()) {
        dealParsePainter(paintPath, cmdLine);
        cmdLine.clear();
      }
    }
    cmdLine += c;
  }
  if (!cmdLine.isEmpty()) {
    dealParsePainter(paintPath, cmdLine);
    cmdLine.clear();
  }
}

void SvgHelper::dealParsePainter(QPainterPath& path, QString line) {
  line = line.trimmed();
  int coordinates;
  QString cmd = line.mid(0, 1);
  QString value = line.mid(1);

  if (cmd.isUpper())
    coordinates = ABSOLUTE_COORDINATES;
  else
    coordinates = RELATIVE_COORDINATES;

  QVector<double> vNum = segmentationCoordinates(value);
  qout << vNum;

  switch (cmdTypeList.indexOf(cmd.toUpper().at(0))) {
    case 0:  // m
    {
      bool hasLineflag = vNum.length() > 2;
      bool lineto = false;

      if (!path.isEmpty())
        svgPathList.append(path);
      path.clear();
      if (!testpathlist.isEmpty())
        svgPointList.append(testpathlist);
      testpathlist.clear();

      while (vNum.length() / 2 >= 1) {
        QPointF point(vNum.at(0), vNum.at(1));
        if (coordinates == ABSOLUTE_COORDINATES) {
          nowPositon = point;
          if (lineto)
            path.lineTo(nowPositon);
          else
            path.moveTo(nowPositon);
          testpathlist.append(nowPositon);
        } else {
          nowPositon = nowPositon + point;
          if (lineto)
            path.lineTo(nowPositon);
          else
            path.moveTo(nowPositon);
          testpathlist.append(nowPositon);
        }
        qout << "nowPositon: " << nowPositon;
        if (!lineto)
          pathStartPosition = nowPositon;

        for (int i = 0; i < 2; i++)
          vNum.removeFirst();
        if (hasLineflag)
          lineto = true;
      }
      break;
    }
    case 1:  // l
    {
      while (vNum.length() / 2 >= 1) {
        QPointF point(vNum.at(0), vNum.at(1));
        if (coordinates == ABSOLUTE_COORDINATES) {
          path.lineTo(point);
          testpathlist.append(point);
          nowPositon = point;
        } else {
          QPointF targetPoint = nowPositon + point;
          path.lineTo(targetPoint);
          testpathlist.append(targetPoint);
          nowPositon = targetPoint;
        }
        qout << "nowPositon: " << nowPositon;
        path.moveTo(nowPositon);
        for (int i = 0; i < 2; i++)
          vNum.removeFirst();
      }
      break;
    }
    case 2:  // h
    {
      while (vNum.length() >= 1) {
        if (coordinates == ABSOLUTE_COORDINATES) {
          QPointF point(vNum.at(0), nowPositon.y());
          path.lineTo(point);
          testpathlist.append(point);
          nowPositon = point;
        } else {
          QPointF point(nowPositon.x() + vNum.at(0), nowPositon.y());
          path.lineTo(point);
          testpathlist.append(point);
          nowPositon = point;
        }
        qout << "nowPositon: " << nowPositon;
        path.moveTo(nowPositon);
        vNum.removeFirst();
      }
      break;
    }
    case 3:  // v
    {
      while (vNum.length() >= 1) {
        if (coordinates == ABSOLUTE_COORDINATES) {
          QPointF point(nowPositon.x(), vNum.at(0));
          path.lineTo(point);
          testpathlist.append(point);
          nowPositon = point;
        } else {
          QPointF point(nowPositon.x(), nowPositon.y() + vNum.at(0));
          path.lineTo(point);
          testpathlist.append(point);
          nowPositon = point;
        }
        qout << "nowPositon: " << nowPositon;
        path.moveTo(nowPositon);
        vNum.removeFirst();
      }
      break;
    }
    case 4:  // c
    {
      while (vNum.length() / 6 >= 1) {
        QPointF startPoint = nowPositon;
        QPointF c1, c2, endPoint;
        if (coordinates == ABSOLUTE_COORDINATES) {
          c1 = QPointF(vNum.at(0), vNum.at(1));
          c2 = QPointF(vNum.at(2), vNum.at(3));
          endPoint = QPointF(vNum.at(4), vNum.at(5));
        } else {
          double x = startPoint.x();
          double y = startPoint.y();
          c1 = QPointF(x + vNum.at(0), y + vNum.at(1));
          c2 = QPointF(x + vNum.at(2), y + vNum.at(3));
          endPoint = QPointF(x + vNum.at(4), y + vNum.at(5));
        }
        lastControlPosition = c2;
        path.moveTo(startPoint);
        path.cubicTo(c1, c2, endPoint);  // 三次贝塞尔曲线
        path.moveTo(endPoint);

        {
          QPainterPath testpath;

          testpath.moveTo(startPoint);
          testpath.cubicTo(c1, c2, endPoint);  // 三次贝塞尔曲线
          testpath.moveTo(endPoint);
          for (double i = 0; i <= 1; i += 1.0 / testpath.length())
            testpathlist.append(testpath.pointAtPercent(i));
        }

        nowPositon = endPoint;
        qout << "nowPositon: " << nowPositon;

        for (int i = 0; i < 6; i++)
          vNum.removeFirst();
      }
      break;
    }
    case 5:  // s
    {
      while (vNum.length() / 4 >= 1) {
        QPointF startPoint = nowPositon;
        QPointF c1, c2, endPoint;
        if (coordinates == ABSOLUTE_COORDINATES) {
          double x = startPoint.x();
          double y = startPoint.y();
          c1 = QPointF(2 * x - lastControlPosition.x(),
                       2 * y - lastControlPosition.y());
          c2 = QPointF(vNum.at(0), vNum.at(1));
          endPoint = QPointF(vNum.at(2), vNum.at(3));
        } else {
          double x = startPoint.x();
          double y = startPoint.y();
          c1 = QPointF(x + x - lastControlPosition.x(),
                       y + y - lastControlPosition.y());
          c2 = QPointF(x + vNum.at(0), y + vNum.at(1));
          endPoint = QPointF(x + vNum.at(2), y + vNum.at(3));
        }
        lastControlPosition = c2;
        path.moveTo(startPoint);
        path.cubicTo(c1, c2, endPoint);  // 三次贝塞尔曲线
        path.moveTo(endPoint);

        {
          QPainterPath testpath;

          testpath.moveTo(startPoint);
          testpath.cubicTo(c1, c2, endPoint);  // 三次贝塞尔曲线
          testpath.moveTo(endPoint);
          for (double i = 0; i <= 1; i += 1.0 / testpath.length())
            testpathlist.append(testpath.pointAtPercent(i));
        }

        nowPositon = endPoint;
        qout << "nowPositon: " << nowPositon;

        for (int i = 0; i < 4; i++)
          vNum.removeFirst();
      }

      break;
    }
    case 6:  // q
    {
      while (vNum.length() / 4 >= 1) {
        QPointF startPoint = nowPositon;
        QPointF cPoint;
        QPointF endPoint;
        if (coordinates == ABSOLUTE_COORDINATES) {
          cPoint = QPointF(vNum.at(0), vNum.at(1));
          endPoint = QPointF(vNum.at(2), vNum.at(3));
        } else {
          double x = startPoint.x();
          double y = startPoint.y();
          cPoint = QPointF(x + vNum.at(0), y + vNum.at(1));
          endPoint = QPointF(x + vNum.at(2), y + vNum.at(3));
        }
        lastControlPosition = cPoint;
        path.moveTo(startPoint);
        path.quadTo(cPoint, endPoint);  // 二次贝塞尔曲线
        path.moveTo(endPoint);

        {
          QPainterPath testpath;

          testpath.moveTo(startPoint);
          testpath.quadTo(cPoint, endPoint);  // 二次贝塞尔曲线
          testpath.moveTo(endPoint);
          for (double i = 0; i <= 1; i += 1.0 / testpath.length())
            testpathlist.append(testpath.pointAtPercent(i));
        }

        nowPositon = endPoint;
        qout << "nowPositon: " << nowPositon;

        for (int i = 0; i < 4; i++)
          vNum.removeFirst();
      }
      break;
    }
    case 7:  // t
    {
      while (vNum.length() / 2 >= 1) {
        QPointF startPoint = nowPositon;
        QPointF cPoint;
        QPointF endPoint;
        if (coordinates == ABSOLUTE_COORDINATES) {
          double x = startPoint.x();
          double y = startPoint.y();
          cPoint = QPointF(2 * x - lastControlPosition.x(),
                           2 * y - lastControlPosition.y());
          endPoint = QPointF(vNum.at(0), vNum.at(1));
        } else {
          double x = startPoint.x();
          double y = startPoint.y();
          cPoint = QPointF(x + x - lastControlPosition.x(),
                           y + y - lastControlPosition.y());
          endPoint = QPointF(x + vNum.at(0), y + vNum.at(1));
        }
        lastControlPosition = cPoint;
        path.moveTo(startPoint);
        path.quadTo(cPoint, endPoint);  // 二次贝塞尔曲线
        path.moveTo(endPoint);

        {
          QPainterPath testpath;

          testpath.moveTo(startPoint);
          testpath.quadTo(cPoint, endPoint);  // 二次贝塞尔曲线
          testpath.moveTo(endPoint);
          for (double i = 0; i <= 1; i += 1.0 / testpath.length())
            testpathlist.append(testpath.pointAtPercent(i));
        }

        nowPositon = endPoint;
        qout << "nowPositon: " << nowPositon;

        for (int i = 0; i < 2; i++)
          vNum.removeFirst();
      }
      break;
    }
    case 8:  // a
    {
      while (vNum.length() / 7 >= 1) {
        QPointF startPoint = nowPositon;
        double rx = vNum.at(0);
        double ry = vNum.at(1);
        double x_axis_rotation = vNum.at(2);
        double large_arc_flag = vNum.at(3);
        double sweep_flag = vNum.at(4);
        QPointF endPoint;

        double cx;
        double cy;
        double start_angle;
        double delta_angle;

        if (coordinates == ABSOLUTE_COORDINATES) {
          endPoint = QPointF(vNum.at(5), vNum.at(6));
        } else {
          double x = startPoint.x();
          double y = startPoint.y();
          endPoint = QPointF(x + vNum.at(5), y + vNum.at(6));
        }

        // svg : [A | a] (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
        // x1 y1 rx ry φ fA fS x2 y2
        svgArcToCenterParam(startPoint.x(), startPoint.y(), rx, ry,
                            x_axis_rotation, large_arc_flag, sweep_flag,
                            endPoint.x(), endPoint.y(), cx, cy, start_angle,
                            delta_angle);

        start_angle = qRadiansToDegrees(start_angle);
        delta_angle = qRadiansToDegrees(delta_angle);

        QRectF rect(cx - rx, cy - ry, 2 * rx, 2 * ry);

        qout << "converted: " << cx << cy << start_angle << delta_angle << rect;

        if (delta_angle != 0) {
          path.moveTo(startPoint);
          path.arcTo(rect, 360 - start_angle,
                     -delta_angle);  // 绘图的坐标系与笛卡尔坐标系不同, 需要转换

          {
            QPainterPath testpath;

            testpath.moveTo(startPoint);
            testpath.arcTo(rect, 360 - start_angle, -delta_angle);
            testpath.moveTo(endPoint);
            for (double i = 0; i <= 1; i += 1.0 / testpath.length())
              testpathlist.append(testpath.pointAtPercent(i));
          }
        }
        path.moveTo(endPoint);

        nowPositon = endPoint;
        qout << "nowPositon: " << nowPositon;

        for (int i = 0; i < 7; i++)
          vNum.removeFirst();
      }

      break;
    }
    case 9:  // z
      qout << "z:" << pathStartPosition;
      path.lineTo(pathStartPosition);
      testpathlist.append(pathStartPosition);
      break;
  }
}

QVector<double> SvgHelper::segmentationCoordinates(QString value) {
  // 将科学记数法暂时替换, 防止分割出错
  if (value.contains("e", Qt::CaseInsensitive)) {
    value.replace("e-", "[KXJSFF]");
    value.replace("E-", "[KXJSFF]");
  }

  if (value.contains(" -")) {
    value.replace(" -", " [KGFS]");
  }

  if (value.contains(",-")) {
    value.replace(",-", ",[DHFS]");
  }

  qout << "------value:------" << value;
  QVector<double> vPos;
  QString num = "";
  foreach (QChar c, value) {
    if (splitList.contains(c)) {
      num = num.trimmed();
      if (!num.isEmpty()) {
        if (num.contains(','))
          num.remove(',');
        vPos.append(getValueWithoutUnit(num));
        num.clear();
      }
    }
    num += c;
  }
  num = num.trimmed();
  if (num.contains(','))
    num.remove(',');
  if (!num.isEmpty())
    vPos.append(getValueWithoutUnit(num));
  qout << "------num:------" << num;
  return vPos;
}

double SvgHelper::getValueWithoutUnit(QString input) {
  // 将科学记数法替换回来
  if (input.contains("[KXJSFF]"))
    input.replace("[KXJSFF]", "e-");
  if (input.contains("[KGFS]"))
    input.replace("[KGFS]", "-");
  if (input.contains("[DHFS]"))
    input.replace("[DHFS]", "-");
  qout << "------input:------" << input;

  if (input.isEmpty())
    return 0;
  QString str = input;
  foreach (QString unit, unitList) {
    if (str.contains(unit)) {
      str.remove(unit);
    }
  }
  return str.toDouble();
}

/**
svg : [A | a] (rx ry x-axis-rotation large-arc-flag sweep-flag x2 y2)+

(x1 y1)圆弧路径起点
(x2 y2)圆弧路径终点

rx 椭圆弧的X半轴长度
ry 椭圆弧的Y半轴长度
x-axis-rotation 椭圆弧X轴方向的旋转角度
large-arc-flag 标记是否大弧段
sweep-flag 标记是否顺时针绘制

sample :  svgArcToCenterParam(200,200,50,50,0,1,1,300,200, output...)
*/
double SvgHelper::radian(double ux, double uy, double vx, double vy) {
  double dot = ux * vx + uy * vy;
  double mod = sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
  double rad = acos(dot / mod);
  if (ux * vy - uy * vx < 0.0)
    rad = -rad;
  return rad;
}

int SvgHelper::svgArcToCenterParam(double x1,
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
                                   double& deltaAngle_out) {
  double cx, cy, startAngle, deltaAngle, endAngle;
  double PIx2 = M_PI * 2.0;

  if (rx < 0) {
    rx = -rx;
  }
  if (ry < 0) {
    ry = -ry;
  }
  if (rx == 0.0 || ry == 0.0) {  // invalid arguments
    return -1;
  }

  double s_phi = sin(phi);
  double c_phi = cos(phi);
  double hd_x = (x1 - x2) / 2.0;  // half diff of x
  double hd_y = (y1 - y2) / 2.0;  // half diff of y
  double hs_x = (x1 + x2) / 2.0;  // half sum of x
  double hs_y = (y1 + y2) / 2.0;  // half sum of y

  // F6.5.1
  double x1_ = c_phi * hd_x + s_phi * hd_y;
  double y1_ = c_phi * hd_y - s_phi * hd_x;

  // F.6.6 Correction of out-of-range radii
  //   Step 3: Ensure radii are large enough
  double lambda = (x1_ * x1_) / (rx * rx) + (y1_ * y1_) / (ry * ry);
  if (lambda > 1) {
    rx = rx * sqrt(lambda);
    ry = ry * sqrt(lambda);
  }

  double rxry = rx * ry;
  double rxy1_ = rx * y1_;
  double ryx1_ = ry * x1_;
  double sum_of_sq = rxy1_ * rxy1_ + ryx1_ * ryx1_;  // sum of square
  if (!sum_of_sq) {
    return -1;
  }
  double coe = sqrt(abs((rxry * rxry - sum_of_sq) / sum_of_sq));
  if (fA == fS) {
    coe = -coe;
  }

  // F6.5.2
  double cx_ = coe * rxy1_ / ry;
  double cy_ = -coe * ryx1_ / rx;

  // F6.5.3
  cx = c_phi * cx_ - s_phi * cy_ + hs_x;
  cy = s_phi * cx_ + c_phi * cy_ + hs_y;

  double xcr1 = (x1_ - cx_) / rx;
  double xcr2 = (x1_ + cx_) / rx;
  double ycr1 = (y1_ - cy_) / ry;
  double ycr2 = (y1_ + cy_) / ry;

  // F6.5.5
  startAngle = radian(1.0, 0.0, xcr1, ycr1);

  // F6.5.6
  deltaAngle = radian(xcr1, ycr1, -xcr2, -ycr2);
  while (deltaAngle > PIx2) {
    deltaAngle -= PIx2;
  }
  while (deltaAngle < 0.0) {
    deltaAngle += PIx2;
  }
  if (fS == 0) {
    deltaAngle -= PIx2;
  }
  endAngle = startAngle + deltaAngle;
  while (endAngle > PIx2) {
    endAngle -= PIx2;
  }
  while (endAngle < 0.0) {
    endAngle += PIx2;
  }

  cx_out = cx;
  cy_out = cy;
  startAngle_out = startAngle;
  deltaAngle_out = deltaAngle;

  return 1;
}

QList<QList<QPointF> > SvgHelper::getSvgPointList() const {
  return svgPointList;
}

QList<QPainterPath> SvgHelper::getSvgPathList() const {
  return svgPathList;
}

/*
    get angle ACB, point C is the center point
    A(x1,y1)
    B(x2,y2)
    C(x3,y3)
*/
double SvgHelper::get_angle_with_points(double x1,
                                        double y1,
                                        double x2,
                                        double y2,
                                        double x3,
                                        double y3) {
  double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
  if (theta > M_PI)
    theta -= 2 * M_PI;
  if (theta < -M_PI)
    theta += 2 * M_PI;

  theta = abs(theta * 180.0 / M_PI);

  if (y2 <= y3) {
    theta = 360.0 - theta;
  }

  return theta;
}