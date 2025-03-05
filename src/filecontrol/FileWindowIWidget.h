// HistoryTableWidget.h
#ifndef HISTORYTABLEWIDGET_H
#define HISTORYTABLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>
#include "../common/HistoryCheckBoxItem.h"

class FileWindowIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWindowIWidget(QWidget* parent = nullptr);
    void addItem(const QString& text, bool checked = false);
    void clear();
	void clearLineEditText();

signals:
    void folderMoved(const QString& newPath);
    void itemRenamed(const QString& oldName, const QString& newName);
    void itemDeleted(const QString& name);

private slots:
    void onMoveFolder();
    void onRename();
    void onDelete();

private:
    void setupUI();

    QListWidget* ui_listWidget;
    QPushButton* ui_selectedToggle;
    QPushButton* ui_btnMoveFolder;
    QPushButton* ui_btnRename;
    QPushButton* ui_btnDelete;
    QPushButton* ui_btnClear;
    QLineEdit* ui_lineEdit;

	bool m_isSelectedAll = false;
};

#endif // HISTORYTABLEWIDGET_H
