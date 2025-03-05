#ifndef IMAGEVIEWCONTAINER_H
#define IMAGEVIEWCONTAINER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QScrollArea>
#include <QFileInfo>
#include <QEvent>
#include <QKeyEvent>
#include <QDir>
#include <QCollator>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QMainWindow>
#include <QScrollBar>
#include "ImageView.h"
#include "ImageSettingProps.h"
#include "../common/StatusStore.h"
#include "../common/FileUtils.h"

class ImageViewContainer : public QWidget
{
    Q_OBJECT

public:

	struct ImageScale {
		int percentage;
		int degree;
		bool isFlip;
		ImageView::ScaleMode scaleMode;
	};

    ImageViewContainer(QWidget* parent = nullptr);
	~ImageViewContainer();
	void initImageFile(QString filePath, FileUtils::MoveMode moveMode);
	bool changeSplitView();
	//void navigateToFile(FileUtils::MoveMode moveMode);
	//void navigateToFolder(FileUtils::MoveMode moveMode);
	void navigate(FileUtils::MoveMode moveMode);
	void resizeImage(ImageView::ScaleMode scaleMode, std::optional<bool> isPlus = std::nullopt);
	void deleteImageFile();
	void clear();
	void toggleFullScreen(bool isNormal = false);
	void rotate(int degree);
	void flip();
	void applySettings();
	bool swapImageBox();

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

signals:
	void deleteKeyPressed(QStringList files, FileUtils::SupportType type);
	void renameFile(QString file);
	void renameFolder(QString file);

private:
	QHBoxLayout* createSlider();
	ImageView::Align getAlign(int containerIdx);

private: //variable
	static constexpr int M_IMAGE_BROWSER_CNT = 2;
	QSlider* ui_qSlider;
	QLabel* ui_qSliderInfo;
	QHBoxLayout* ui_hBoxBrowser;
	ImageView* ui_imageView[M_IMAGE_BROWSER_CNT];
	FileUtils::viewerFile m_imageInfo[M_IMAGE_BROWSER_CNT];
	ImageScale m_imageScale;

};

#endif // IMAGEVIEWCONTAINER_H
