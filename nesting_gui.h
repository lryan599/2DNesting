#pragma once

#include <QtCharts>
#include <QtWidgets/QMainWindow>
#include "csv.h"
#include "data_converter.h"
#include "layout_widget.h"
#include "table_manager.h"
#include "ui_create_sheet.h"
#include "ui_nesting_gui.h"
#include "ui_wait.h"
#include "worker.h"

class nesting_gui : public QMainWindow {
  Q_OBJECT

 public:
  nesting_gui(QWidget* parent = nullptr);
  void startWork(
      const bool need_simplify,
      const double top_offset,
      const double left_offset,
      const double bottom_offset,
      const double right_offset,
      const double part_offset,
      const double sheet_width,
      const double sheet_height,
      const size_t max_time,
      const std::vector<nesting::geo::Polygon_with_holes_2>& polygons,
      const std::vector<uint32_t>& items_rotations,
      const std::vector<uint32_t>& items_quantity);
  void storeData(std::vector<nesting::geo::Polygon_with_holes_2> pgns,
                 std::vector<std::uint32_t> rots,
                 std::vector<std::uint32_t> quat);
  ~nesting_gui();
 public slots:
  void createSheet();
  void openCSV();
  void openCAD();
  void openSVG();
  void toTXT();
  void toDXF();
  void toSVG();
  void start();
  void stop();
  void handleProgress(Solution,
                      const std::vector<nesting::Item>&,
                      const std::vector<nesting::Item>&);
  void handleFinished();
  void handleMessage(const QString&);

 private:
  Ui::nesting_guiClass ui;
  Ui::CreateSheetDialog UICreateSheetDialog;
  Ui::WaitDialog UIWaitDialog;
  QDialog* waitDialog{nullptr};
  QChart* chart;
  QLineSeries* series;
  QValueAxis* axisX;
  QValueAxis* axisY;
  QTimer* timer;
  uint64_t time{0};
  Worker* worker{nullptr};
};
