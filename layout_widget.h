#pragma once

#include <qdebug.h>
#include <QApplication>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPainter>
#include <QTableWidget>
#include <QWheelEvent>
#include <QPolygon>

class LayoutWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  LayoutWidget(QWidget* parent = nullptr) : QOpenGLWidget(parent) {}

  void initialize_transform();
  void set_sheet(qreal w, qreal h);
 public slots:
  void layoutUpdate(QTableWidgetItem* n, QTableWidgetItem* o);

 protected:
  void initializeGL() override;

  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

  void paintEvent(QPaintEvent* event) override;

  void wheelEvent(QWheelEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

 private:
  QPointF m_lastPos;
  QTransform globalTransform;
  QRectF sheet;
  QList<QPolygonF> layout;
  qreal length{-1};
};