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
			qDebug() << "same index " << value;
		}
		else {
			qDebug() << "slider value changed : " << value;
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
		initImageFile(filePath, FileUtils::PrevFolder);
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
	ui_qSliderInfo->setText(QString("count: %1 / %2").arg(files.at(0).currentIndex + 1).arg(ui_qSlider->maximum()));

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
	//resizeImage(m_imageScale.scaleMode, m_imageScale.percentage);
	return newSplit;

}

/*void ImageViewContainer::navigateToFolder(FileUtils::MoveMode moveMode) {

	const ImageListInfo& imageInfo = m_imageInfo;

	if (imageInfo.fileList.isEmpty() ||
		imageInfo.currentIndex >= imageInfo.fileList.size() || imageInfo.currentIndex < 0) {
		return;
	}

	QString filePath = imageInfo.fileList.at(imageInfo.currentIndex);
	QString file;

	if (moveMode == FileUtils::NextFolder) {
		file = FileUtils::moveFolder(filePath, FileUtils::MoveMode::NextFolder, FileUtils::IMAGE);
	}
	else if (moveMode == FileUtils::PrevFolder) {
		file = FileUtils::moveFolder(filePath, FileUtils::MoveMode::PrevFolder, FileUtils::IMAGE);
	}

	if (file.isEmpty()) {
		QMessageBox::warning(this, "warning", "can not find next/prev image folder.");
		return;
	}

	loadFileList(file);

}*/
/*
void ImageViewContainer::navigateToFile(FileUtils::MoveMode moveMode) {

	ImageListInfo* imageInfo = &m_imageInfo;
	const ImageScale& imageScale = m_imageScale;


	bool isSplit = StatusStore::instance().getImageSettings().isSplitView();
	// split 모드이면 2씩 이동, 아니면 1씩 이동
	int step = isSplit ? 2 : 1;

	int currentIndex = imageInfo->currentIndex;
	// 이동 모드에 따른 인덱스 변경
	if (moveMode == FileUtils::Next) {
		currentIndex += step;
	}
	else if (moveMode == FileUtils::Prev) {
		currentIndex -= step;
	}
	else if (moveMode == FileUtils::First) {
		currentIndex = 0;
	}
	else if (moveMode == FileUtils::Last) {
		currentIndex = imageInfo->fileList.size() - 1;
	}
	// moveMode가 None인 경우 m_currentIndex는 그대로 사용

	// 자동 다음 페이지 이동.
	if (currentIndex < 0 && StatusStore::instance().getImageSettings().isAutoNext()) {
		navigateToFolder(FileUtils::PrevFolder);
		return;
	}
	else if (currentIndex >= imageInfo->fileList.size() && StatusStore::instance().getImageSettings().isAutoNext()) {
		navigateToFolder(FileUtils::NextFolder);
		return;
	}else if (currentIndex < 0 || currentIndex >= imageInfo->fileList.size()) {
		return;
	}

	// 분할 모드인 경우 첫 번째 이미지는 짝수 인덱스가 되도록 보정
	if (isSplit && (currentIndex % 2) != 0 && currentIndex > 0) {
			currentIndex--;
		}
	QString fileName[M_IMAGE_BROWSER_CNT];
	for (int i = 0; i < M_IMAGE_BROWSER_CNT; i++) {
		if (ui_imageView[i]->isVisible() && currentIndex + i < imageInfo->fileList.size()) {
			fileName[i] = imageInfo->fileList.at(currentIndex + i);
			ui_imageView[i]->loadImage(fileName[i], imageScale.scaleMode, imageScale.percentage, getAlign(i));
		}
		else {
			ui_imageView[i]->clear();
		}
	}

	imageInfo->currentIndex = currentIndex;
	ui_qSlider->setValue(imageInfo->currentIndex);
	this->window()->setWindowTitle(QString("SzViewer - %1       /        %2").arg(fileName[0]).arg(fileName[1]));
}
*/
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
		else if (keyEvent->key() == Qt::Key_F2 && !(keyEvent->modifiers() & Qt::ControlModifier)) {
			// m_imageInfo.fileList = renameFile(m_imageInfo.fileList, m_imageInfo.currentIndex, 0, m_imageScale);
		}
		else if (keyEvent->key() == Qt::Key_F3 && StatusStore::instance().getImageSettings().isSplitView()) {
			//  m_imageInfo.fileList = renameFile(m_imageInfo.fileList, m_imageInfo.currentIndex+1, 1, m_imageScale);
		}
		else if (keyEvent->key() == Qt::Key_F2 && keyEvent->modifiers() & Qt::ControlModifier) {
			/* QString newPath = renameFolder(m_imageInfo.fileList, m_imageInfo.currentIndex);
			 if (newPath.isEmpty() == false)
			 {
				 initImageFile(newPath, );
			 }*/
		}
		else if (keyEvent->key() == Qt::Key_N) {
			ui_hBoxBrowser->setDirection(ui_hBoxBrowser->direction() == QBoxLayout::LeftToRight
				? QBoxLayout::RightToLeft
				: QBoxLayout::LeftToRight);
			applySettings();
		}
		else if (keyEvent->key() == Qt::Key_BracketLeft) {
			/*m_imageInfo.fileList.insert(m_imageInfo.currentIndex, "");
			navigateToFile(FileUtils::None);
			ui_qSlider->setMaximum(m_imageInfo.fileList.size() - 1);*/

		}
		else if (keyEvent->key() == Qt::Key_BracketRight) {
			/*m_imageInfo.fileList.insert(m_imageInfo.currentIndex + 1, "");
			navigateToFile(FileUtils::None);
			ui_qSlider->setMaximum(m_imageInfo.fileList.size() - 1);*/
		}
		return false;  // 이벤트를 가로채서 처리 완료
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

QStringList ImageViewContainer::renameFile(QStringList fileList, int fileIdx, int containerIdx, ImageScale imageScale) {
	if (fileList.isEmpty() || fileIdx >= fileList.size()) {
		return fileList;
	}

	QString currentFile = fileList.at(fileIdx);
	QFileInfo fileInfo(currentFile);
	QString oldFileName = fileInfo.completeBaseName();
	QString suffix = fileInfo.suffix();

	bool ok;
	QString newFileName = QInputDialog::getText(this, "(Rename File)", "please new file name:", QLineEdit::Normal, oldFileName, &ok);

	if (ok && !newFileName.isEmpty()) {
		ui_imageView[containerIdx]->movieStop();
		QDir dir = fileInfo.dir();
		QString newPath = dir.filePath(newFileName + "." + suffix);

		if (QFile::rename(currentFile, newPath)) {
			// 파일명 변경 성공
			m_imageInfo[containerIdx].fileName = newPath;
			fileList[fileIdx] = newPath;
			return fileList;
		}
		else {
			QMessageBox::warning(this, "warning", "can not rename.");
		}
	}
	return fileList;
}

QString ImageViewContainer::renameFolder(QStringList fileList, int fileIdx) {
	if (fileList.isEmpty() || fileIdx >= fileList.size()) {
		return QString();
	}

	QString currentFile = fileList.at(fileIdx);
	QFileInfo fileInfo(currentFile);
	QDir dir = fileInfo.dir();
	QString oldFolderName = dir.dirName();
	dir.cdUp();
	QString parentPath = dir.absolutePath();

	bool ok;
	QString newFolderName = QInputDialog::getText(this, "[Rename Folder]", "please new folder name:", QLineEdit::Normal, oldFolderName, &ok);

	if (ok && !newFolderName.isEmpty()) {
		QString o = QDir::cleanPath(parentPath + QDir::separator() + oldFolderName);
		QString n = QDir::cleanPath(parentPath + QDir::separator() + newFolderName);
		ui_imageView[0]->movieStop();
		ui_imageView[1]->movieStop();
		qDebug() << " o : " << o << " n : " << n;
		if (fileInfo.dir().rename(o, n)) {
			return QDir::cleanPath(parentPath + QDir::separator() + newFolderName + QDir::separator() + fileInfo.fileName());
		}
		else {
			QMessageBox::warning(this, "warning", "can not rename.");
		}
	}
	return QString();
}

void ImageViewContainer::deleteImageFile() {

	QStringList files;

	for (int i = 0;i < M_IMAGE_BROWSER_CNT;i++) {
		if (m_imageInfo[i].fileName.isEmpty() == false) {
			files.append(m_imageInfo[0].fileName);
		}
	}

	emit deleteKeyPressed(files);
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