#include "SzViewer.h"

SzViewer::SzViewer(QWidget* parent)
	: QMainWindow(parent)
{

	//창 설정.
	QSettings settings("SzViewer", "SzViewer-Common");
	QRect geom = settings.value("geometry", QRect(100, 100, 1024, 768)).toRect();
	setGeometry(geom);

	ui_stackedWidget = new QStackedWidget(this);

	ui_textViewContainer = new TextViewContainer(this);
	ui_stackedWidget->addWidget(ui_textViewContainer);
	connect(ui_textViewContainer, &TextViewContainer::deleteKeyPressed, this, &SzViewer::handleDeleteKey);
	connect(ui_textViewContainer, &TextViewContainer::renameFile, this, [this](QString fileName) {
		openFile(FileUtils::renameFile(fileName, this));
		});

	ui_imageViewContainer = new ImageViewContainer(this);
	ui_stackedWidget->addWidget(ui_imageViewContainer);
	connect(ui_imageViewContainer, &ImageViewContainer::deleteKeyPressed, this, &SzViewer::handleDeleteKey);
	connect(ui_imageViewContainer, &ImageViewContainer::renameFile, this, [this](QString fileName) {
		openFile(FileUtils::renameFile(fileName, this));
		});
	connect(ui_imageViewContainer, &ImageViewContainer::renameFolder, this, [this](QString folderName) {
		openFile(FileUtils::renameFolder(folderName, this));
		});

	this->setCentralWidget(ui_stackedWidget);

	// 드래그 앤 드롭 활성화
	setAcceptDrops(true);

	QToolBar* leftToolBar = CommonLeft();
	ui_textToolBar = new TextToolBar(this, ui_textViewContainer);
	ui_imageToolBar = new ImageToolBar(this, ui_imageViewContainer);
	QToolBar* rightToolBar = CommonRight();

	// 두 툴바를 한 줄에 배치할 컨테이너 위젯 생성
	QWidget* topToolBarContainer = new QWidget(this);
	QHBoxLayout* topToolBarLayout = new QHBoxLayout(topToolBarContainer);
	topToolBarLayout->setContentsMargins(0, 0, 0, 0);
	topToolBarLayout->setSpacing(0);
	topToolBarLayout->setAlignment(Qt::AlignLeft);

	topToolBarLayout->addWidget(leftToolBar, 0);
	topToolBarLayout->addWidget(ui_textToolBar, 0);
	topToolBarLayout->addWidget(ui_imageToolBar, 0);
	topToolBarLayout->addWidget(rightToolBar, 0);

	// QMainWindow의 메뉴바 영역에 배치 (메뉴바 영역으로 사용)
	this->setMenuWidget(topToolBarContainer);

	changeVisible(false);

}

SzViewer::~SzViewer()
{
	QSettings settings("SzViewer", "SzViewer-Common");
	settings.setValue("geometry", this->geometry());
}

void SzViewer::handleDeleteKey(QStringList files, FileUtils::SupportType type) {
	QStringList fileList = FileUtils::getFileList(files.at(0), type);

	QString prevFolder = FileUtils::moveFolder(files.at(0), FileUtils::MoveMode::Prev, type);
	QString nextFolder = FileUtils::moveFolder(files.at(0), FileUtils::MoveMode::Next, type);


	DeleteFilesDialog dialog(files, m_deleteFolder, this);
	if (dialog.exec() == QDialog::Accepted) {
		this->window()->setWindowTitle(QString("SzViewer"));
		QStringList deletedData = dialog.getDeletedFiles();
		if (dialog.isDeleteFolderChecked() && !deletedData.isEmpty()) {
			openFile(nextFolder.isEmpty() ? prevFolder : nextFolder);
		}
		else if(!deletedData.isEmpty()){
			//deletedData는 파일 정렬 순서대로 삭제되어 넘어온다. last index가 마지막 삭제된 파일.
			//다음 파일이 존재하면 다음 파일로 이동 
			int nextIdx = fileList.indexOf(deletedData.at(deletedData.size() - 1)) + 1;
			if (nextIdx < fileList.size()) {
				openFile(fileList.at(nextIdx));
				return;
			}
			//다음 파일이 없고. 이전 파일이 존재한다면 (마지막 파일 삭제시) 
			int prevIdx = fileList.indexOf(deletedData.at(0)) - 1;
			if (prevIdx >= 0) {
				openFile(fileList.at(prevIdx));
				return;
			}

			//모든 파일이 삭제된 상태라면 초기화
			if (type == FileUtils::SupportType::IMAGE) {
				ui_imageViewContainer->clear();
			}
			else if (type == FileUtils::SupportType::TEXT) {
				ui_textViewContainer->clear();
			}
		}
	}
	else {
		//openFile(files[0]);
		qDebug() << "delete cancel";
	}
}

QToolBar* SzViewer::CommonLeft() {

	QToolBar* toolBar = new QToolBar(this);
	toolBar->setMovable(true);

	// QToolButton과 QMenu를 사용하여 파일 메뉴를 구성합니다.
	QToolButton* fileToolButton = new QToolButton(toolBar);
	fileToolButton->setText("File");
	fileToolButton->setIcon(QIcon(":/icon/resources/icon/folder_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"));
	fileToolButton->setPopupMode(QToolButton::InstantPopup);

	QMenu* fileMenu = new QMenu(fileToolButton);

	// 첫번째 줄: Open 액션
	QAction* openAction = fileMenu->addAction("Open");
	connect(openAction, &QAction::triggered, this, &SzViewer::openFileDialog);

	QAction* historyClearAction = fileMenu->addAction("Clear");
	connect(historyClearAction, &QAction::triggered, this, [this, fileMenu]() {

		StatusStore::instance().getTextHistory().removeAllNoFixed();
		StatusStore::instance().saveTextHistory(StatusStore::instance().getTextHistory());

		StatusStore::instance().getImageHistory().removeAllNoFixed();
		StatusStore::instance().saveImageHistory(StatusStore::instance().getImageHistory());
		addQActionInFileMenu(fileMenu);

		});


	connect(fileMenu, &QMenu::aboutToShow, [this, fileMenu]() {
		addQActionInFileMenu(fileMenu);
		});

	fileToolButton->setMenu(fileMenu);
	toolBar->addWidget(fileToolButton);

	return toolBar;
}

void SzViewer::addQActionInFileMenu(QMenu* fileMenu) {
	QList<QAction*> actions = fileMenu->actions();
	// 1,2 액션은 삭제하지 않음
	for (int i = actions.size() - 1; i > 1; i--) {
		fileMenu->removeAction(actions.at(i));
	}

	HistoryProps textProps = StatusStore::instance().getTextHistory();
	addHistoryCheckBox(fileMenu, textProps, HistoryProps::SavedType::TEXT_HISTORY);

	HistoryProps imgProps = StatusStore::instance().getImageHistory();
	addHistoryCheckBox(fileMenu, imgProps, HistoryProps::SavedType::IMAGE_HISTORY);
}

void SzViewer::addHistoryCheckBox(QMenu* fileMenu, HistoryProps props, HistoryProps::SavedType type) {

	fileMenu->addSeparator();

	QList<SavedFileInfo> history = props.getFileInfos().values();
	std::sort(history.begin(), history.end(), [](const SavedFileInfo& a, const SavedFileInfo& b) {
		return a.savedDateTime > b.savedDateTime;
		});

	for (int i = 0; i < history.size(); i++) {
		SavedFileInfo saveFileInfo = history.at(i);
		QString fileName = saveFileInfo.fileName;
		if (fileName.isEmpty()) {
			continue;
		}

		HistoryCheckBoxItem* checkBox = new HistoryCheckBoxItem(fileName, fileMenu);
		checkBox->setChecked(saveFileInfo.isBookmarked);

		QWidgetAction* widgetAction = new QWidgetAction(fileMenu);
		widgetAction->setDefaultWidget(checkBox);
		fileMenu->addAction(widgetAction);

		//체크박스 클릭 시 이벤트 처리
		connect(checkBox, &HistoryCheckBoxItem::indicatorClicked, [this, fileName, saveFileInfo, type](bool checked) {
			StatusStore::instance().savedFixedState(type, fileName, checked);
			});

		//라벨 클릭 시 이벤트 처리
		connect(checkBox, &HistoryCheckBoxItem::labelClicked, [this, fileName, fileMenu]() {
			openFile(fileName);
			fileMenu->close();  // 팝업 닫기
			});
	}

}

QToolBar* SzViewer::CommonRight() {
	QToolBar* toolBar = new QToolBar(this);
	toolBar->setMovable(true);
	QAction* imageAction = new QAction(QIcon(":/icon/resources/icon/autorenew_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.svg"), "changeView", this);
	connect(imageAction, &QAction::triggered, this, [this]() { changeVisible(!ui_imageViewContainer->isVisible());});
	toolBar->addAction(imageAction);

	QAction* helpAction = new QAction(QIcon(":/icon/resources/icon/help_outline_24dp_1F1F1F.svg"), "Help", this);
	connect(helpAction, &QAction::triggered, this, [this]() {
		AboutDialog dialog(this);
		dialog.exec();
		});
	toolBar->addAction(helpAction);

	return toolBar;
}

void SzViewer::changeVisible(bool isCurrentTextView) {
	if (isCurrentTextView) {
		ui_stackedWidget->setCurrentWidget(ui_imageViewContainer);
		ui_imageToolBar->setVisible(true);
		ui_imageViewContainer->setVisible(true);
		ui_textToolBar->setVisible(false);
		ui_textViewContainer->setVisible(false);
	}
	else {
		ui_stackedWidget->setCurrentWidget(ui_textViewContainer);
		ui_imageToolBar->setVisible(false);
		ui_imageViewContainer->setVisible(false);
		ui_textToolBar->setVisible(true);
		ui_textViewContainer->setVisible(true);
	}
}

void SzViewer::openFileDialog()
{
	if (ui_textViewContainer->isVisible()) {
		QString fileName = QFileDialog::getOpenFileName(this, "Open File",
			QDir::currentPath(), "Text Files (*.txt)");
		openFile(fileName);
	}
	else {
		QString fileName = QFileDialog::getOpenFileName(this, "Open File",
			QDir::currentPath(), "Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.webp)");

		openFile(fileName);
	}

}

void SzViewer::resizeEvent(QResizeEvent* event) {
	if (ui_imageViewContainer->isVisible()) {
		//todo
	}
	else if (ui_textViewContainer->isVisible()) {
		ui_textViewContainer->refreshPage(ui_textViewContainer->getFileInfo()->currentPosition);
	}
}

// 드래그 엔터 이벤트 처리 : 파일이면 받아들임.
void SzViewer::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

// 드랍 이벤트 처리 : 첫 번째 파일 경로를 사용하여 파일 오픈.
void SzViewer::dropEvent(QDropEvent* event)
{
	QList<QUrl> urls = event->mimeData()->urls();

	if (urls.isEmpty() || urls.length() != 1) {
		return;
	}

	openFile(urls.first().toLocalFile());
}

void SzViewer::openFile(QString fileName) {
	if (fileName.isEmpty()) {
		return;
	}

	QString findFileName = FileUtils::findFileInSubDir(fileName);
	ZipArchiveManager& zipManager = ZipArchiveManager::instance();

	if (FileUtils::isSupportSuffix(fileName, FileUtils::ZIP)) {
		zipManager.openArchive(fileName);
		bool isImage = false;
		bool isText = false;
		for (int i = 0; i < zipManager.getFileList().size(); i++) {
			QString fileName = zipManager.getFileList().at(i);
			if (FileUtils::isSupportSuffix(fileName, FileUtils::IMAGE)) {
				isImage = true;
			}
			else if (FileUtils::isSupportSuffix(fileName, FileUtils::TEXT)) {
				isText = true;
			}

			if (isImage && isText) {
				break;
			}
		}
		if (!isImage && !isText) {
			QMessageBox::warning(this, "warning", "wrong zip file. " + fileName);
			return;
		}
		findFileName = zipManager.getFileList().at(0);
	}


	if (FileUtils::isSupportSuffix(findFileName, FileUtils::IMAGE)) {
		// 예: 이미지 파일을 처리하는 로직 추가
		// 예를 들면 이미지 뷰어 위젯에 이미지를 표시하는 방식 등이 있을 수 있음.
		changeVisible(true);
		ui_imageViewContainer->initImageFile(findFileName, FileUtils::None);
	}
	else if (FileUtils::isSupportSuffix(findFileName, FileUtils::TEXT)) {
		changeVisible(false);
		ui_textViewContainer->initTextFile(findFileName, FileUtils::None);
	}
	else {
		QMessageBox::warning(this, "warning", "can not support image/text file. " + fileName);
	}
}
