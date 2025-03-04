#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include "common/HistoryProps.h"
#include "textviewer/TextViewContainer.h"
#include "textviewer/TextToolBar.h"
#include "AboutDialog.h"
#include "common/DeleteFilesDialog.h"
#include "imageviewer/ImageViewContainer.h"
#include "imageviewer/ImageToolbar.h"
#include "common/HistoryCheckBoxItem.h"
#include "common/FileUtils.h"
#include "common/ZipArchiveManager.h"

#include <QStackedWidget>
#include <QSettings>
#include <QFileDialog>
#include <QToolButton>
#include <QMimeData>
#include <QWidgetAction>
#include <QMessageBox>

class SzViewer : public QMainWindow
{
    Q_OBJECT

public:
    SzViewer(QWidget *parent = nullptr);
    ~SzViewer();


protected:
    void resizeEvent(QResizeEvent* event);
    void dropEvent(QDropEvent* event);
	void dragEnterEvent(QDragEnterEvent* event);
    QToolBar* CommonLeft();
    QToolBar* CommonRight();
    void changeVisible(bool isCurrentTextView);

private slots:
    void openFileDialog();
	void handleDeleteKey(QStringList files);

private:
    bool isTextFile(const QString& fileName);
    void openFile(QString fileName);
    void addHistoryCheckBox(QMenu* fileMenu, HistoryProps props, HistoryProps::SavedType type);
    void addQActionInFileMenu(QMenu* fileMenu);

 //variable
private:
    bool m_deleteFolder = false;
    TextViewContainer* ui_textViewContainer;
    ImageViewContainer* ui_imageViewContainer;
    QStackedWidget* ui_stackedWidget;
    TextToolBar* ui_textToolBar;
    ImageToolBar* ui_imageToolBar;
};