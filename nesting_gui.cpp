#include "nesting_gui.h"

nesting_gui::nesting_gui(QWidget* parent) : QMainWindow(parent) {
  ui.setupUi(this);
  // Part1 设置各表格自动拉伸
  ui.partsTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui.sheetsTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui.partsTable->setTargetColumn(2);
  ui.sheetsTable->setTargetColumn(2);
  ui.resultTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  // Part2 设置QToolButton的action
  ui.partsOpenCAD->setDefaultAction(ui.actionOpen_DXF_DWG_File);
  ui.partsOpenCSV->setDefaultAction(ui.actionOpen_CSV_File);
  ui.partsOpenSVG->setDefaultAction(ui.actionOpen_SVG_File);
  ui.CreateSheetButton->setDefaultAction(ui.actionCreate_Sheet);
  ui.ExportTXTButton->setDefaultAction(ui.actionTo_TXT_File);
  ui.ExportSVGButton->setDefaultAction(ui.actionTo_SVG_File);
  ui.ExportDXFButton->setDefaultAction(ui.actionTo_DXF_File);
  // Part3 设置Nesting Tab中各控件的初始状态
  ui.fixRun->setChecked(true);
  ui.StopButton->setDisabled(true);
  ui.progressBar->setValue(0);
  // Part4 设置图表对象

  // 坐标轴
  axisX = new QValueAxis();
  axisY = new QValueAxis();
  axisX->setRange(0, 60);
  axisX->setTickCount(7);
  axisY->setRange(0, 1);
  axisX->setTitleText("Time(s)");
  axisY->setTitleText("Util(%)");
  // 值
  series = new QLineSeries();
  series->setName("Util(%)");
  series->useOpenGL();
  // series->setPointsVisible(true);
  //  图像
  chart = new QChart();
  chart->addAxis(axisX, Qt::AlignBottom);
  chart->addAxis(axisY, Qt::AlignLeft);
  chart->addSeries(series);
  // chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->legend()->hide();
  series->attachAxis(axisX);
  series->attachAxis(axisY);

  ui.resultChart->setRenderHint(QPainter::Antialiasing);
  ui.resultChart->setChart(chart);
  ui.resultChart->setRubberBand(QChartView::VerticalRubberBand);
  // 滚动条
  ui.ChartScrollBar->setRange(0, 30);
  connect(ui.ChartScrollBar, &QScrollBar::valueChanged, [&](int value) {
    // 计算偏移量
    static bool scrolling = false;
    static int previous = 0;
    // 在水平方向上滚动
    if (!scrolling) {
      scrolling = true;
      ui.resultChart->chart()->axisX()->setRange(value, value + 60);
      previous = value;
      scrolling = false;
    }
  });
  // Part5 设置CurrentLayout
  // 设置dockwidget只在tabindex=2时显现
  connect(ui.tabWidget, &QTabWidget::currentChanged, ui.dockWidget,
          [&](int index) {
            if (index != 2) {
              ui.dockWidget->hide();
            } else {
              ui.dockWidget->show();
            }
          });
  ui.dockWidget->setWidget(ui.openGLWidget);
  ui.dockWidget->hide();
  this->addDockWidget(Qt::BottomDockWidgetArea, ui.dockWidget);
  this->resizeDocks({ui.dockWidget}, {(int)(this->height() * 0.5)},
                    Qt::Vertical);
  // Part7 设置时钟
  timer = new QTimer(this);

  connect(timer, &QTimer::timeout, [&]() {
    // 处理progressBar
    qDebug() << "timer START";
    auto max = ui.progressBar->maximum();
    auto min = ui.progressBar->minimum();
    if (max != 0 || min != 0) {
      if (time > max) {
        if (timer->isActive()) {
          qDebug() << "stop timer 2";
          ui.StopButton->click();
        }
      } else {
        ui.progressBar->setValue(time + 1);
      }
    }
    // 处理Chart
    if (ui.resultTable->rowCount() > 0) {
      series->append(
          time, ui.resultTable->item(0, 1)->data(Qt::DisplayRole).toReal());
    } else {
      series->append(time, 0);
    }
    ui.ChartScrollBar->setMaximum(ui.ChartScrollBar->maximum() + 1);
    ++time;
    qDebug() << "timer END";
  });
}

nesting_gui::~nesting_gui() {
  delete chart;
  delete series;
  delete axisX;
  delete axisY;
  delete timer;
}

void nesting_gui::createSheet() {
  auto rowIndex = ui.sheetsTable->rowCount();
  if (rowIndex >= 1) {
    QMessageBox::warning(this, "Warning",
                         "Currently, only one sheet is allowed to be used.");
    return;
  }
  ui.tabWidget->setCurrentIndex(1);
  QDialog dialog(this);
  UICreateSheetDialog.setupUi(&dialog);
  UICreateSheetDialog.label_3->setVisible(false);
  UICreateSheetDialog.quantity->setVisible(false);
  if (QDialog::Accepted == dialog.exec()) {
    auto width = UICreateSheetDialog.width->value();
    auto height = UICreateSheetDialog.height->value();
    auto quantity = UICreateSheetDialog.quantity->value();
    ui.sheetsTable->insertRow(rowIndex);
    auto i1 = new QTableWidgetItem();
    i1->setData(Qt::DisplayRole, width);
    ui.sheetsTable->setItem(rowIndex, 0, i1);
    auto i2 = new QTableWidgetItem();
    i2->setData(Qt::DisplayRole, height);
    ui.sheetsTable->setItem(rowIndex, 1, i2);
    auto i4 = new QTableWidgetItem();
    QPolygonF p({QPointF(0, 0), QPointF(width, 0), QPointF(width, height),
                 QPointF(0, height)});
    i4->setData(Qt::DisplayRole, p);
    ui.sheetsTable->setItem(rowIndex, 2, i4);
  }
}

void nesting_gui::openCSV() {
  ui.tabWidget->setCurrentIndex(0);
  QString filePath =
      QFileDialog::getOpenFileName(this, "Open CSV File", ".", "CSV (*.csv)");
  if (filePath.isEmpty()) {
    return;
  }
  std::vector<nesting::geo::Polygon_with_holes_2> pgns;
  std::vector<std::uint32_t> rots;
  std::vector<std::uint32_t> quat;
  try {
    nesting::read_from_csv(filePath.toStdString(), pgns, rots, quat);
  } catch (const std::runtime_error& e) {
    QMessageBox::warning(this, "Open CSV File", e.what());
  }
  storeData(pgns, rots, quat);
}

void nesting_gui::openCAD() {
  ui.tabWidget->setCurrentIndex(0);
  QStringList filePaths = QFileDialog::getOpenFileNames(
      this, "Open DXF/DWG File", ".", "CAD (*.dxf *.dwg)");
  if (filePaths.isEmpty()) {
    return;
  }
  std::vector<nesting::geo::Polygon_with_holes_2> pgns;
  std::vector<std::uint32_t> rots;
  std::vector<std::uint32_t> quat;
  for (auto& qstring : filePaths) {
    try {
      nesting::read_from_cad(qstring.toStdString(), pgns, rots, quat);
    } catch (const std::runtime_error& e) {
      QMessageBox::warning(this, "Open CAD File", e.what());
    }
  }
  storeData(pgns, rots, quat);
}

void nesting_gui::openSVG() {
  ui.tabWidget->setCurrentIndex(0);
  QStringList filePaths =
      QFileDialog::getOpenFileNames(this, "Open SVG File", ".", "SVG (*.svg)");
  if (filePaths.isEmpty()) {
    return;
  }
  std::vector<nesting::geo::Polygon_with_holes_2> pgns;
  std::vector<std::uint32_t> rots;
  std::vector<std::uint32_t> quat;
  for (auto& qstring : filePaths) {
    try {
      nesting::read_from_svg(qstring.toStdString(), pgns, rots, quat);
    } catch (const std::runtime_error& e) {
      QMessageBox::warning(this, "Open SVG File", e.what());
    }
  }
  storeData(pgns, rots, quat);
}

void nesting_gui::toTXT() {
  if (ui.resultTable->rowCount() == 0) {
    QMessageBox::warning(this, "Warning", "Solutions do not exist");
    return;
  }
  QString filePath = QFileDialog::getSaveFileName(this, "Save File", ".",
                                                  "Text files (*.txt)");
  if (!filePath.isEmpty()) {
    auto i1 = ui.resultTable->item(0, 0);
    auto i2 = ui.resultTable->item(0, 1);
    auto i3 = ui.resultTable->item(0, 2);
    auto util = i2->data(Qt::DisplayRole).value<double>();
    auto pgns = i2->data(Qt::UserRole)
                    .value<std::vector<nesting::geo::Polygon_with_holes_2>>();
    auto sheet_length =
        ui.sheetsTable->item(0, 0)->data(Qt::DisplayRole).value<double>();
    auto sheet_width =
        ui.sheetsTable->item(0, 1)->data(Qt::DisplayRole).value<double>();
    auto ret = nesting::write_to_txt(filePath.toStdString(), util, sheet_width,
                                     sheet_length, pgns);
    if (ret) {
      QMessageBox::information(this, "Success", "File saved successfully");
    } else {
      QMessageBox::warning(this, "Fail", "File failed to save");
    }
  }
}

void nesting_gui::toDXF() {
  if (ui.resultTable->rowCount() == 0) {
    QMessageBox::warning(this, "Warning", "Solutions do not exist");
    return;
  }
  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", ".", "DXF (*.dxf)");
  if (!filePath.isEmpty()) {
    auto i1 = ui.resultTable->item(0, 0);
    auto i2 = ui.resultTable->item(0, 1);
    auto i3 = ui.resultTable->item(0, 2);
    auto util = i2->data(Qt::DisplayRole).value<double>();
    auto pgns = i2->data(Qt::UserRole)
                    .value<std::vector<nesting::geo::Polygon_with_holes_2>>();
    auto sheet_length =
        ui.sheetsTable->item(0, 0)->data(Qt::DisplayRole).value<double>();
    auto sheet_width =
        ui.sheetsTable->item(0, 1)->data(Qt::DisplayRole).value<double>();
    auto ret = nesting::write_to_dxf(filePath.toStdString(), util, sheet_width,
                                     sheet_length, pgns);
    if (ret) {
      QMessageBox::information(this, "Success", "File saved successfully");
    } else {
      QMessageBox::warning(this, "Fail", "File failed to save");
    }
  }
}

void nesting_gui::toSVG() {
  if (ui.resultTable->rowCount() == 0) {
    QMessageBox::warning(this, "Warning", "Solutions do not exist");
    return;
  }
  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", ".", "SVG (*.svg)");
  if (!filePath.isEmpty()) {
    auto i1 = ui.resultTable->item(0, 0);
    auto i2 = ui.resultTable->item(0, 1);
    auto i3 = ui.resultTable->item(0, 2);
    auto util = i2->data(Qt::DisplayRole).value<double>();
    auto pgns = i2->data(Qt::UserRole)
                    .value<std::vector<nesting::geo::Polygon_with_holes_2>>();
    auto sheet_length =
        ui.sheetsTable->item(0, 0)->data(Qt::DisplayRole).value<double>();
    auto sheet_width =
        ui.sheetsTable->item(0, 1)->data(Qt::DisplayRole).value<double>();
    auto ret = nesting::write_to_svg(filePath.toStdString(), util, sheet_width,
                                     sheet_length, pgns);
    if (ret) {
      QMessageBox::information(this, "Success", "File saved successfully");
    } else {
      QMessageBox::warning(this, "Fail", "File failed to save");
    }
  }
}

void nesting_gui::start() {
  // 清空上次的ui
  qDebug() << "start";
  ui.progressBar->setRange(0, 100);
  ui.progressBar->setValue(0);
  series->clear();
  ui.resultTable->clearContents();
  ui.resultTable->setRowCount(0);
  auto parts_count = ui.partsTable->rowCount();
  if (parts_count == 0) {
    QMessageBox::warning(this, "Warning", "Parts do not exsit");
    return;
  }
  auto sheets_count = ui.sheetsTable->rowCount();
  if (sheets_count == 0) {
    QMessageBox::warning(this, "Warning", "Sheet does not exsit");
    return;
  }
  // 获取part
  std::vector<uint32_t> allowed_rotations;
  std::vector<uint32_t> quantity;
  std::vector<nesting::geo::Polygon_with_holes_2> polygons;
  for (size_t i = 0; i < parts_count; i++) {
    QTableWidgetItem* i0 = ui.partsTable->item(i, 0);
    QTableWidgetItem* i1 = ui.partsTable->item(i, 1);
    QTableWidgetItem* i2 = ui.partsTable->item(i, 2);
    auto allowed_rotation = i0->data(Qt::DisplayRole).value<uint32_t>();
    auto qua = i1->data(Qt::DisplayRole).value<uint32_t>();
    auto pgn =
        i2->data(Qt::UserRole).value<nesting::geo::Polygon_with_holes_2>();
    allowed_rotations.push_back(allowed_rotation);
    quantity.push_back(qua);
    polygons.push_back(pgn);
  }
  // 获取sheet
  QTableWidgetItem* i0 = ui.sheetsTable->item(0, 0);
  QTableWidgetItem* i1 = ui.sheetsTable->item(0, 1);
  double sheet_width = i0->data(Qt::DisplayRole).value<double>();
  double sheet_height = i1->data(Qt::DisplayRole).value<double>();
  ui.openGLWidget->set_sheet(sheet_width, sheet_height);
  // 时间参数
  int seconds = 24 * 3600;
  if (ui.fixRun->isChecked()) {
    auto time = ui.timeEdit->time();
    auto h = time.hour();
    auto m = time.minute();
    auto s = time.second();
    seconds = (h * 3600 + m * 60 + s);
    ui.progressBar->setRange(0, seconds);
  } else {
    ui.progressBar->setRange(0, 0);
  }
  // 其他参数
  auto need_simplify = ui.needSimplify->isChecked();
  auto part_offset = ui.partSpacingSpinBox->value();
  auto top_offset = ui.TopSpinBox->value();
  auto bottom_offset = ui.BottomSpinBox->value();
  auto left_offset = ui.LeftSpinBox->value();
  auto right_offset = ui.RightSpinBox->value();
  // ui更新
  ui.ChartScrollBar->setRange(0, 30);
  ui.ChartScrollBar->setValue(0);
  ui.resultChart->chart()->axisX()->setRange(0, 60);
  ui.startButton->setDisabled(true);
  ui.StopButton->setEnabled(true);
  ui.timeEdit->setDisabled(true);
  ui.partSpacingSpinBox->setDisabled(true);
  ui.TopSpinBox->setDisabled(true);
  ui.LeftSpinBox->setDisabled(true);
  ui.BottomSpinBox->setDisabled(true);
  ui.RightSpinBox->setDisabled(true);
  ui.fixRun->setDisabled(true);
  ui.needSimplify->setDisabled(true);
  // 开启时钟
  time = 0;
  timer->start(1000);
  startWork(need_simplify, top_offset, left_offset, bottom_offset, right_offset,
            part_offset, sheet_width, sheet_height, seconds, polygons,
            allowed_rotations, quantity);
}

void nesting_gui::stop() {
  qDebug() << "stop START";
  // 关闭定时任务
  if (timer->isActive()) {
    timer->stop();
  }
  // 强制停止线程
  if (worker != nullptr && worker->isRunning()) {
    waitDialog = new QDialog(this);
    UIWaitDialog.setupUi(waitDialog);
    worker->quit();
    worker->requestQuit();
    waitDialog->setWindowFlag(Qt::WindowCloseButtonHint, false);
    waitDialog->open();
    QEventLoop loop;
    connect(worker, &QThread::finished, &loop, &QEventLoop::quit);
    loop.exec();
  }
  qDebug() << "stop END";
}

void nesting_gui::handleProgress(Solution solu,
                                 const std::vector<nesting::Item>& sim,
                                 const std::vector<nesting::Item>& ori) {
  qDebug() << "handleProgress START";
  qDebug() << solu.length;
  qDebug() << solu.utilization;
  // Part1 更新result table
  auto rowIndex = ui.resultTable->rowCount();
  ui.resultTable->insertRow(0);

  auto i2 = new QTableWidgetItem();
  i2->setData(Qt::DisplayRole, solu.utilization);
  ui.resultTable->setItem(0, 1, i2);
  auto i3 = new QTableWidgetItem();
  i3->setData(Qt::DisplayRole, solu.time);
  ui.resultTable->setItem(0, 2, i3);
  auto part_offset = ui.partSpacingSpinBox->value();
  auto bottom_offset = ui.BottomSpinBox->value();
  auto left_offset = ui.LeftSpinBox->value();
  auto i1 = new QTableWidgetItem();
  auto pgns = nesting::postprocess(solu.solution, part_offset, left_offset, bottom_offset, sim, ori);
  QVariant var1;
  var1.setValue(pgns);
  i2->setData(Qt::UserRole, var1);
  auto size = pgns.size();
  QList<QPolygonF> display_pgns;
  for (size_t i = 0; i < size; i++) {
    QPolygonF polygon;
    auto& pwh = pgns[i];
    auto& outer = pwh.outer_boundary();
    for (auto v = outer.vertices_begin(); v != outer.vertices_end(); v++) {
      polygon.append(QPointF(CGAL::to_double(v->x()), CGAL::to_double(v->y())));
    }
    display_pgns.append(polygon);
  }
  QVariant var2;
  var2.setValue(display_pgns);
  i1->setData(Qt::UserRole, var2);
  i1->setData(Qt::DisplayRole, solu.length);
  ui.resultTable->setItem(0, 0, i1);
  ui.resultTable->setCurrentCell(0, 0);
  // Part2 更新result chart
  series->append(solu.time, solu.utilization);
  qDebug() << "handleProgress END";
}

void nesting_gui::startWork(
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
    const std::vector<uint32_t>& items_quantity) {
  // Part6 设置线程
  qDebug() << "start work START";
  worker = new Worker(this);
  connect(worker, &Worker::resultReady, this, &nesting_gui::handleProgress);
  connect(worker, &Worker::finished, this, &nesting_gui::handleFinished);
  connect(worker, &Worker::sendMessage, this, &nesting_gui::handleMessage);
  worker->set(need_simplify, top_offset, left_offset, bottom_offset,
             right_offset, part_offset, sheet_width, sheet_height, max_time,
             polygons, items_rotations, items_quantity);
  worker->start();
  qDebug() << "start work END";
}

void nesting_gui::storeData(
    std::vector<nesting::geo::Polygon_with_holes_2> pgns,
    std::vector<std::uint32_t> rots,
    std::vector<std::uint32_t> quat) {
  auto numRows = pgns.size();
  ui.partsTable->setRowCount(numRows);
  for (size_t i = 0; i < numRows; ++i) {
    auto i1 = new QTableWidgetItem();
    i1->setData(Qt::DisplayRole, rots[i]);
    ui.partsTable->setItem(i, 0, i1);
    auto i2 = new QTableWidgetItem();
    i2->setData(Qt::DisplayRole, quat[i]);
    ui.partsTable->setItem(i, 1, i2);
    auto i3 = new QTableWidgetItem();
    qreal px = 0;
    qreal py = 0;
    QPolygonF pgn;
    for (auto& v : pgns[i].outer_boundary()) {
      px = CGAL::to_double(v.x());
      py = CGAL::to_double(v.y());
      pgn.append(QPointF(px, py));
    }
    i3->setData(Qt::DisplayRole, pgn);
    QVariant var;
    var.setValue(pgns[i]);
    i3->setData(Qt::UserRole, var);
    ui.partsTable->setItem(i, 2, i3);
  }
}

void nesting_gui::handleFinished() {
  qDebug() << "handleFinished()";
   //线程对象应该会自动释放
  if (worker != nullptr) {
    qDebug() << worker->isFinished();
    delete worker;
    worker = nullptr;
  }
  if (waitDialog != nullptr) {
    waitDialog->close();
    delete waitDialog;
    waitDialog = nullptr;
  }
  ui.StopButton->click();
  QMessageBox::information(this, "Info", "Engine thread exited");
  qDebug() << "handleFinished() END";
}

void nesting_gui::handleMessage(const QString& s) {
  qDebug() << "handleMessage(): " << s;
  QMessageBox::critical(this, "Error", s);
}
