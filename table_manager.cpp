#include "table_manager.h"

TableManager::TableManager(QWidget* parent) {
  polygonDelegate.setParent(this);
}

TableManager::~TableManager() {}

void TableManager::setTargetColumn(int c) {
  setItemDelegateForColumn(c, &polygonDelegate);
}

void TableManager::exportTableData() {}

void TableManager::deleteSeleceted() {
  QList<QTableWidgetItem*> selectedItems = this->selectedItems();
  // 根据选定的项获取行号
  QList<int> rowsToRemove;
  for (auto item : selectedItems) {
    int row = item->row();
    if (!rowsToRemove.contains(row)) {
      rowsToRemove.append(row);
    }
  }
  // 从QTableWidget中删除选定的行
  std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());
  for (auto row : rowsToRemove) {
    this->removeRow(row);
  }
}