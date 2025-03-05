#ifndef FILEWINDOWWIDGET_H
#define FILEWINDOWWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include "../common/HistoryCheckBoxItem.h"
#include "../common/FileUtils.h"

class FileWindowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWindowWidget(QWidget* parent = nullptr);
    void addItem(const QString& text, bool checked = false);
    void clear();
	void clearLineEditText();

signals:
    void folderMoved(const QString& newPath);
    void itemRenamed(const QString& oldName, const QString& newName);
	void onItemDoubleClick(const QString& fileName);

private slots:
    void onMoveFolder();
    void onRename();

private:
    void setupUI();
    void sortItems();
    QList<QListWidgetItem*> getCheckedItem();
    bool hasRenameSpecialChar(QString pattern);
    void setLineText(QString text, QString color);
    QListWidget* ui_listWidget;
    QLineEdit* ui_lineEdit;

	bool m_isSelectedAll = false;
};

#endif // FILEWINDOWWIDGET_H
