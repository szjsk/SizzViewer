// FileWindowContainer.h
#ifndef FILEWINDOWCONTAINER_H
#define FILEWINDOWCONTAINER_H

#include <QDialog>
#include <QVBoxLayout>
#include "FileWindowWidget.h"
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
    FileWindowWidget* getTableWidget(int index);
	void appendFile(int keyEvent, QString fileName);

signals:
    void onItemDoubleClicked(int index, const QString& fileName); 

private:
    QVBoxLayout* ui_mainLayout;
    QList<FileWindowWidget*> ui_tableWidgets;
};

#endif // FILEWINDOWCONTAINER_H
