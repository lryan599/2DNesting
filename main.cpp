#include <QtWidgets/QApplication>
#include "nesting_gui.h"

int main(int argc, char* argv[]) {
  // 设置 OpenGL 版本
  QSurfaceFormat format;
  format.setProfile(QSurfaceFormat::CoreProfile);  // 使用核心配置
  QSurfaceFormat::setDefaultFormat(format);
  QApplication a(argc, argv);
  nesting_gui w;
  w.show();
  return a.exec();
}
