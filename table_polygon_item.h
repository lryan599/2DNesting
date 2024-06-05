#pragma once

#include <qpainter.h>
#include <qstyleditemdelegate.h>

class TablePolygonDelegate : public QStyledItemDelegate {
 public:
  TablePolygonDelegate(QObject* parent = nullptr)
      : QStyledItemDelegate(parent) {}

  // 设置不可编辑
  QWidget* createEditor(QWidget* parent,
                        const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override {
    return nullptr;
  }

  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const override {
    if (index.isValid()) {
      // 获取表格单元格的坐标和尺寸
      QRect rect = option.rect;
      auto polygon = index.data(Qt::DisplayRole).value<QPolygonF>();
      // qDebug() << polygon;
      painter->save();
      // 计算多边形的包围矩形
      QRectF polygonRect = polygon.boundingRect();

      // 计算放缩比例，使得多边形的比例不变
      qreal scaleX = (rect.width() - 10) / polygonRect.width();
      qreal scaleY = (rect.height() - 10) / polygonRect.height();
      qreal scale = qMin(scaleX, scaleY);
      // 缩放多边形
      QPolygonF scaledPolygon;
      for (const QPointF& point : polygon) {
        scaledPolygon.append(QPointF(point.x() * scale, point.y() * scale));
      }
      QPointF center = rect.center() - scaledPolygon.boundingRect().center();

      // 开始绘制
      painter->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
      painter->setBrush(Qt::darkYellow);               // 设置画刷颜色

      // 在矩形内绘制多边形
      painter->translate(center);
      // painter->scale(scale, scale);
      painter->drawPolygon(scaledPolygon);
      painter->restore();
    }
  }
};
