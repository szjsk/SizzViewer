#ifndef IMAGETOOLBAR_H
#define IMAGETOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include "../common/StatusStore.h"
#include "ImageViewContainer.h"
#include "ImageSettingDialog.h"

class ImageToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ImageToolBar(QWidget* parent = nullptr, ImageViewContainer* container = nullptr);

private:
	void move(ImageViewContainer* container, FileUtils::MoveMode mode);
	void openSettingDialog(ImageViewContainer* container);

private slots:

};

#endif // IMAGETOOLBAR_H