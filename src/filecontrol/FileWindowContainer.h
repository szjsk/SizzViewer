// FileWindowContainer.h
#ifndef FILEWINDOWCONTAINER_H
#define FILEWINDOWCONTAINER_H

#include <QDialog>
#include <QVBoxLayout>
#include "FileWindowIWidget.h"
#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QSettings>

class FileWindowContainer : public QDialog
{
    Q_OBJECT

public:
    explicit FileWindowContainer(QWidget* parent = nullptr);
    ~FileWindowContainer();
    void addTableWidget();
    FileWindowIWidget* getTableWidget(int index);
	void appendFile(int keyEvent, QString fileName);

private:
    QVBoxLayout* ui_mainLayout;
    QList<FileWindowIWidget*> ui_tableWidgets;
};

#endif // FILEWINDOWCONTAINER_H
