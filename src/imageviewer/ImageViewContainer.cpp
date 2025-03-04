#include "ImageViewContainer.h"
#include "../common/FileUtils.h"



ImageViewContainer::ImageViewContainer(QWidget* parent)
	: QWidget(parent)
{

	//main
	QVBoxLayout* vBoxContainer = new QVBoxLayout(this);

	ui_hBoxBrowser = new QHBoxLayout();
	ui_hBoxBrowser->setContentsMargins(0, 0, 0, 0);  // 패딩을 0으로 설정
	ui_hBoxBrowser->setSpacing(0);  // 간격을 0으로 설정

	// hBoxBrowser에 두 스크롤 영역 추가
	ui_imageView[0] = new ImageView(this);
	ui_hBoxBrowser->addWidget(ui_imageView[0]);
	ui_imageView[1] = new ImageView(this);
	ui_hBoxBrowser->addWidget(ui_imageView[1]);

	ui_imageView[1]->setVisible(StatusStore::instance().getImageSettings().isSplitView());

	vBoxContainer->addLayout(ui_hBoxBrowser, 1);
	//for slider
	vBoxContainer->addLayout(createSlider());
	setLayout(vBoxContainer);
	this->installEventFilter(this);

	//init
	m_imageScale.percentage = 100;
	m_imageScale.scaleMode = StatusStore::instance().getImageSettings().getScaleMode();
	m_imageScale.degree = 0;
	m_imageScale.isFlip = false;

	// 슬라이더 값 변경 시 정보를 업데이트하는 람다 슬롯 연결
	connect(ui_qSlider, &QSlider::valueChanged, this, [this](int value) {

		ui_qSliderInfo->setText(QString("count: %1 / %2").arg(value+1).arg(ui_qSlider->maximum()));

		if (m_imageInfo[0].currentIndex == value) {
			return;
		}
		else {
			int newValue = m_imageInfo[0].fileList.size() <= value ? m_imageInfo[0].fileList.size() - 1 : value;
			initImageFile(m_imageInfo[0].fileList.at(newValue), FileUtils::MoveMode::None);
		}
		});
}

ImageViewContainer::~ImageViewContainer() {
	//StatusStore::instance().saveImageHistory(StatusStore::instance().getImageHistory());
}

void ImageViewContainer::clear() {
	ui_imageView[0]->clear();
	ui_imageView[1]->clear();
	m_imageInfo[0] = FileUtils::SzViewerFile();
	m_imageInfo[1] = FileUtils::SzViewerFile();
	ui_qSlider->setValue(0);
	ui_qSlider->setMaximum(0);
	ui_qSliderInfo->setText(QString("count: %1 / %2").arg(0).arg(0));
}

QHBoxLayout* ImageViewContainer::createSlider() {
	QHBoxLayout* hBoxSlider = new QHBoxLayout();
	ui_qSlider = new QSlider(Qt::Horizontal, this);
	hBoxSlider->addWidget(ui_qSlider);

	ui_qSliderInfo = new QLabel(this);
	hBoxSlider->addWidget(ui_qSliderInfo);

	return hBoxSlider;
}


void ImageViewContainer::initImageFile(QString filePath, FileUtils::MoveMode moveMode) {
	bool isSplit = StatusStore::instance().getImageSettings().isSplitView();
	QList<FileUtils::SzViewerFile> files = FileUtils::extractFileListBy(filePath, moveMode, FileUtils::IMAGE, isSplit);

	// 자동 다음 페이지 이동.
	if (moveMode == FileUtils::Next && files.isEmpty() && StatusStore::instance().getImageSettings().isAutoNext()) {
		initImageFile(filePath, FileUtils::NextFolder);
		return;
	}
	else if (moveMode == FileUtils::Prev && files.isEmpty() && StatusStore::instance().getImageSettings().isAutoNext()) {
		initImageFile(filePath, FileUtils::PrevFolderLastFile);
		return;
	}
	else if (files.isEmpty()) {
		return;
	}
	m_imageScale.degree = 0;
	m_imageScale.isFlip = false;

	int currentIndex = files.at(0).currentIndex;
	int size = files.at(0).size;

	for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
		if (i < files.size()) {
			m_imageInfo[i] = files.at(i);
			ui_imageView[i]->loadImage(files.at(i).fileDataCache, m_imageInfo[i].fileName, m_imageScale.scaleMode, m_imageScale.percentage, getAlign(i));
			QString historyFile = files.at(i).isArchive ? files.at(i).archiveName : files.at(i).fileName;
			StatusStore::instance().getImageHistory().addFileInfo(historyFile, -1, "");
		}
		else {
			m_imageInfo[i] = FileUtils::SzViewerFile();
			ui_imageView[i]->clear();
		}
	}
	ui_qSlider->setMaximum(files.at(0).size);
	ui_qSlider->setValue(files.at(0).currentIndex);
	ui_qSliderInfo->setText(QString("count: %1 / %2").arg(files.at(0).currentIndex+1).arg(ui_qSlider->maximum()));

	this->window()->setWindowTitle(QString("SzViewer - %1       /        %2").arg(m_imageInfo[0].fileName).arg(m_imageInfo[1].fileName));

	this->window()->activateWindow();
	this->window()->raise();
}

void ImageViewContainer::resizeImage(ImageView::ScaleMode scaleMode, std::optional<bool> isPlus) {
	m_imageScale.scaleMode = scaleMode;

	if (m_imageScale.scaleMode == ImageView::ScaleByPercentage) {
		m_imageScale.percentage = (m_imageScale.percentage <= 0) ? 100 : m_imageScale.percentage;

		m_imageScale.percentage = (isPlus.has_value() && isPlus.value()) ? m_imageScale.percentage + 10 : m_imageScale.percentage - 10;

		if (m_imageScale.percentage > 500) {
			m_imageScale.percentage = 500;
		}
		else if (m_imageScale.percentage < 10) {
			m_imageScale.percentage = 10;
		}

		for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
			if (ui_imageView[i]->isVisible()) {
				ui_imageView[i]->resize(m_imageScale.scaleMode, m_imageScale.percentage);
			}
		}
	}
	else {
		m_imageScale.percentage = 100;
		for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
			if (ui_imageView[i]->isVisible()) {
				ui_imageView[i]->resize(m_imageScale.scaleMode, m_imageScale.percentage);
			}
		}
	}
}

bool ImageViewContainer::changeSplitView() {
	bool newSplit = !ui_imageView[1]->isVisible();
	StatusStore::instance().getImageSettings().setSplitView(newSplit);

	ui_imageView[1]->setVisible(newSplit);
	initImageFile(m_imageInfo[0].fileName, FileUtils::None);

	return newSplit;

}

ImageView::Align ImageViewContainer::getAlign(int containerIdx) {

	int newIdx = ui_hBoxBrowser->direction() == QBoxLayout::LeftToRight ? containerIdx : containerIdx - 1;

	ImageView::Align align = ImageView::Align::CENTER;
	if (StatusStore::instance().getImageSettings().isSplitView()
		&& StatusStore::instance().getImageSettings().getAlign() == ImageView::CENTER_SPREAD) {
		align = (newIdx == 0) ? ImageView::Align::RIGHT : ImageView::Align::LEFT;
	}
	else if (StatusStore::instance().getImageSettings().getAlign() == ImageView::CENTER_SPREAD) {
		align = ImageView::Align::CENTER;
	}
	else {
		align = StatusStore::instance().getImageSettings().getAlign();
	}
	return align;
}

void ImageViewContainer::applySettings() {
	//change settings 
	for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
		ui_imageView[i]->setAlignment(getAlign(i));
	}
}

void ImageViewContainer::toggleFullScreen(bool isNormal) {
	QMainWindow* mainWindow = qobject_cast<QMainWindow*>(window());

	//isNormal(esc키)이 true일때는 무조건 노말스크린으로 이동.
	//다만 현재 노말스크린일때는 굳이 로직을 탈필요가 없음. 
	if (isNormal || mainWindow->isFullScreen()) {
		if (mainWindow->isFullScreen()) {
			mainWindow->showNormal();
			ui_qSlider->show();
			ui_qSliderInfo->show();
			mainWindow->menuWidget()->show();
			resizeImage(ImageView::ScaleMode::FitIfLARGE);
			return;
		}
	}
	else {
		ui_qSlider->hide();
		ui_qSliderInfo->hide();
		mainWindow->menuWidget()->hide();
		mainWindow->showFullScreen();
		resizeImage(ImageView::ScaleMode::FitIfLARGE);
		return;
	}

}

void ImageViewContainer::navigate(FileUtils::MoveMode moveMode) {
	initImageFile(m_imageInfo[0].fileName, moveMode);
}

bool ImageViewContainer::eventFilter(QObject* watched, QEvent* event) {

	if (this->isVisible() == false) {
		return QWidget::eventFilter(watched, event);
	}

	if (event->type() == QEvent::KeyRelease) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Escape) {
			toggleFullScreen(true);
		}
		else if (keyEvent->key() == Qt::Key_F) {
			toggleFullScreen();
		}
		else if (keyEvent->key() == Qt::Key_PageDown) {
			navigate(FileUtils::NextFolder);
		}
		else if (keyEvent->key() == Qt::Key_PageUp) {
			navigate(FileUtils::PrevFolder);
		}
		else if (keyEvent->key() == Qt::Key_Left) {
			navigate(FileUtils::Prev);
		}
		else if (keyEvent->key() == Qt::Key_Right) {
			navigate(FileUtils::Next);
		}
		else if (keyEvent->key() == Qt::Key_1) {
			resizeImage(ImageView::ScaleMode::FitToWindow);
		}
		else if (keyEvent->key() == Qt::Key_2) {
			resizeImage(ImageView::ScaleMode::ORIGINAL);
		}
		else if (keyEvent->key() == Qt::Key_3) {
			resizeImage(ImageView::ScaleMode::FitToWidth);
		}
		else if (keyEvent->key() == Qt::Key_4) {
			resizeImage(ImageView::ScaleMode::FitToHeight);
		}
		else if (keyEvent->key() == Qt::Key_5) {
			resizeImage(ImageView::ScaleMode::FitIfLARGE);
		}
		else if ((keyEvent->key() == Qt::Key_Plus)) {
			resizeImage(ImageView::ScaleMode::ScaleByPercentage, true);
		}
		else if ((keyEvent->key() == Qt::Key_Minus)) {
			resizeImage(ImageView::ScaleMode::ScaleByPercentage, false);
		}
		else if (keyEvent->key() == Qt::Key_Delete) {
			deleteImageFile();
		}
		else if (keyEvent->key() == Qt::Key_F2) {
			if (StatusStore::instance().getImageSettings().isSplitView()) {
				QMessageBox::warning(this, "warning", "can not rename in spitMode");
				return false;
			}
			QString fileName = m_imageInfo[0].isArchive ? m_imageInfo[0].archiveName : m_imageInfo[0].fileName;
			emit renameFile(fileName);
		}
		else if (keyEvent->key() == Qt::Key_F3 /* && keyEvent->modifiers() & Qt::ControlModifier*/) {
			QString fileName = m_imageInfo[0].isArchive ? m_imageInfo[0].archiveName : m_imageInfo[0].fileName;
			emit renameFolder(fileName);
		}
		else if (keyEvent->key() == Qt::Key_N) {
			swapImageBox();
		}
		else if (keyEvent->key() == Qt::Key_M) {
			FileUtils::setAddEmptyPage(!FileUtils::isAddEmptyPage());
			navigate(FileUtils::None);

		}
		return false; 
	}


	if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint mousePos = static_cast<QWidget*>(watched)->mapTo(this, mouseEvent->pos());

		int centerX = this->width() / 2;

		if (mousePos.x() < centerX) {
			navigate(FileUtils::Prev);
		}
		else {
			navigate(FileUtils::Next);
		}
	}

	return QWidget::eventFilter(watched, event);
}

bool ImageViewContainer::swapImageBox() {

	bool isSwap = ui_hBoxBrowser->direction() == QBoxLayout::LeftToRight;

	ui_hBoxBrowser->setDirection(isSwap ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);
	applySettings();
	return isSwap;
}


void ImageViewContainer::deleteImageFile() {

	QStringList files;

	if (m_imageInfo[0].fileName.isEmpty()) {
		return;
	}
	else if (m_imageInfo[0].isArchive) {
		files.append(m_imageInfo[0].archiveName);
	}
	else {
		for (int i = 0;i < M_IMAGE_BROWSER_CNT;i++) {
			if (m_imageInfo[i].fileName.isEmpty() == false) {
				files.append(m_imageInfo[i].fileName);
			}
		}
	}

	emit deleteKeyPressed(files, FileUtils::IMAGE);
}

void ImageViewContainer::rotate(int degree) {

	m_imageScale.degree += degree;

	if (m_imageScale.degree == 360 || m_imageScale.degree == -360) {
		m_imageScale.degree = 0;
	}

	for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
		ui_imageView[i]->rotate(m_imageScale.degree, m_imageScale.isFlip);
	}
}

void ImageViewContainer::flip() {
	m_imageScale.isFlip = !m_imageScale.isFlip;
	for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
		ui_imageView[i]->rotate(m_imageScale.degree, m_imageScale.isFlip);
	}
}