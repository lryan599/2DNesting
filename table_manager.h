#pragma once

#include <qtablewidget.h>
#include "table_polygon_item.h"

class TableManager : public QTableWidget {
  Q_OBJECT
 public:
  TableManager(QWidget* parent = nullptr);
  ~TableManager();
  void setTargetColumn(int);
 public slots:
  void deleteSeleceted();
  void exportTableData();
 private:
  TablePolygonDelegate polygonDelegate;
};
