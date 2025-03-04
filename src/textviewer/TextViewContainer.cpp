#include "TextViewContainer.h"

TextViewContainer::TextViewContainer(QWidget* parent)
	: QWidget(parent)
{
	//main
	QVBoxLayout* vBoxContainer = new QVBoxLayout(this);

	TextSettingProps textSetting = StatusStore::instance().getTextSettings();

	//for textBrowser
	QHBoxLayout* hBoxBrowser = new QHBoxLayout();
	ui_TextBrowsers[0] = createTextBrowser(textSetting);
	ui_TextBrowsers[1] = createTextBrowser(textSetting);
	ui_TextBrowsers[0]->installEventFilter(this);

	hBoxBrowser->addWidget(ui_TextBrowsers[0]);
	hBoxBrowser->addWidget(ui_TextBrowsers[1]);

	ui_TextBrowsers[1]->setVisible(textSetting.isSplitView());

	vBoxContainer->addLayout(hBoxBrowser, 1);
	//for slider
	QHBoxLayout* hBoxSlider = new QHBoxLayout();
	ui_QSlider = new QSlider(Qt::Horizontal, this);
	hBoxSlider->addWidget(ui_QSlider);

	ui_QSliderInfo = new QLabel(this);
	hBoxSlider->addWidget(ui_QSliderInfo);

	vBoxContainer->addLayout(hBoxSlider);

	setLayout(vBoxContainer);

	// 슬라이더 값 변경 시 정보를 업데이트하는 람다 슬롯 연결
	connect(ui_QSlider, &QSlider::valueChanged, this, [this](int value) {
		int newVal = setPage(&m_fileInfo, value);
		ui_QSliderInfo->setText(QString("page: %1 / %2").arg(newVal).arg(ui_QSlider->maximum()));
		});

}

TextViewContainer::~TextViewContainer() {
	//saveHistory(StatusStore::instance().getTextHistory(), &m_fileInfo);
}

void TextViewContainer::clear() {
	ui_TextBrowsers[0]->clear();
	ui_TextBrowsers[1]->clear();
	m_fileInfo = TextViewContainer::TextFileInfo();
	ui_QSlider->setValue(0);
	ui_QSlider->setMaximum(0);
	ui_QSliderInfo->setText(QString("page: %1 / %2").arg(0).arg(0));
}


QTextBrowser* TextViewContainer::createTextBrowser(TextSettingProps settings) {

	QTextBrowser* tb = new QTextBrowser(this);
	refreshStyle(settings, tb);

	tb->installEventFilter(this);
	tb->setTextInteractionFlags(Qt::NoTextInteraction);  // 텍스트 상호작용 비활성화
	tb->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 세로 스크롤바 비활성화

	return tb;
}

bool TextViewContainer::isUtf8Text(QByteArray& data) {

	// 인코딩 감지
	bool isUtf8 = true;
	int length = qMin(1000, data.size());
	for (int i = 0; i < length; ++i) {
		if ((data[i] & 0x80) == 0x00) {
			// 0xxxxxxx
			continue;
		}
		else if ((data[i] & 0xE0) == 0xC0) {
			// 110xxxxx 10xxxxxx
			if (i + 1 < data.size() && (data[i + 1] & 0xC0) == 0x80) {
				++i;
				continue;
			}
		}
		else if ((data[i] & 0xF0) == 0xE0) {
			// 1110xxxx 10xxxxxx 10xxxxxx
			if (i + 2 < data.size() && (data[i + 1] & 0xC0) == 0x80 && (data[i + 2] & 0xC0) == 0x80) {
				i += 2;
				continue;
			}
		}
		else if ((data[i] & 0xF8) == 0xF0) {
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			if (i + 3 < data.size() && (data[i + 1] & 0xC0) == 0x80 && (data[i + 2] & 0xC0) == 0x80 && (data[i + 3] & 0xC0) == 0x80) {
				i += 3;
				continue;
			}
		}
		isUtf8 = false;
		break;
	}
	return isUtf8;
}

void TextViewContainer::initTextFile(QString filePath, FileUtils::MoveMode moveMode) {
	QList<FileUtils::SzViewerFile> files = FileUtils::extractFileListBy(filePath , moveMode, FileUtils::TEXT, false);

	if (files.size() != 1) {
		return;
	}
	
	FileUtils::SzViewerFile file = files.at(0);

	this->window()->setWindowTitle(QString("SzViewer - %1").arg(file.fileName));
	m_fileInfo = TextViewContainer::TextFileInfo();

	if (file.isArchive) {
		StatusStore::instance().getTextHistory().addFileInfo(file.archiveName, -1, "");
	}

	SavedFileInfo history = StatusStore::instance().getTextHistory().getFileInfo(file.fileName);


	if (isUtf8Text(file.fileDataCache)) {
		m_fileInfo.text = QString::fromUtf8(file.fileDataCache);
	}
	else {
		m_fileInfo.text = QString::fromLocal8Bit(file.fileDataCache);
	}

	m_fileInfo.fileName = file.fileName;
	m_fileInfo.currentPageIdx = 0;
	refreshPage(history.textPosition);
}

void TextViewContainer::refreshPage(long textPosition) {

	m_fileInfo.pageInfos = calculatePage(&m_fileInfo, getMaxHeight(ui_TextBrowsers[0]), getMaxWidth(ui_TextBrowsers[0]), ui_TextBrowsers[0]);

	int currentPage = findTextPageBy(&m_fileInfo, textPosition);
	if (ui_TextBrowsers[1]->isVisible()) {
		currentPage = currentPage - (currentPage % M_TEXT_BROWSER_CNT);
	}
	m_fileInfo.currentPageIdx = currentPage;
	ui_QSlider->setRange(0, m_fileInfo.pageInfos.size() - 1);
	ui_QSlider->setValue(0);

	if (currentPage == 0) {
		int newVal = setPage(&m_fileInfo, currentPage);
		ui_QSliderInfo->setText(QString("page: %1 / %2").arg(newVal).arg(ui_QSlider->maximum()));
	}
	else {
		ui_QSlider->setValue(currentPage);
	}

	this->window()->activateWindow();
	this->window()->raise();
}

int TextViewContainer::findTextPageBy(const TextFileInfo* fileInfo, long position) {
	if (position <= 0) {
		return 0;
	}

	for (int i = 0;i < fileInfo->pageInfos.size();i++) {
		if (fileInfo->pageInfos.value(i).firstPosition >= position) {
			return i;
		}
	}

	return 0;
}

void TextViewContainer::findPage(const QString& text, long page, long line) {
	int browserIndex = 0;
	if (ui_TextBrowsers[1]->isVisible()) {
		browserIndex = (page % 2);
	}

	ui_QSlider->setValue(page - browserIndex);

	QTextCursor cursor(ui_TextBrowsers[browserIndex]->document());
	cursor = ui_TextBrowsers[browserIndex]->document()->find(text, cursor);
	if (!cursor.isNull()) {
		ui_TextBrowsers[browserIndex]->setTextCursor(cursor);
	}

}

/*
	Style Setting
*/

void TextViewContainer::changeStyle(TextSettingProps s) {

	refreshStyle(s, ui_TextBrowsers[0]);
	refreshStyle(s, ui_TextBrowsers[1]);

}

void TextViewContainer::refreshStyle(TextSettingProps settings, QTextBrowser* tb) {
	QFont f = settings.getFont();
	f.setHintingPreference(QFont::PreferFullHinting);
	f.setStyleStrategy(QFont::PreferAntialias);
	tb->setFont(f);

	QFontMetrics fm(settings.getFont());
	int lineHeight = fm.lineSpacing();

	QString style = QString("QTextBrowser { "
		"background-color: %1; "
		"padding: %2px %3px %4px %5px; "
		"color: %6; }")
		.arg(settings.getBackgroundColor().name())
		.arg(settings.getPadding().top())
		.arg(settings.getPadding().right())
		.arg(settings.getPadding().bottom())
		.arg(settings.getPadding().left())
		.arg(settings.getTextColor().name())
		;

	tb->setStyleSheet(style);
	applyLineSpacing(tb);

}

void TextViewContainer::applyLineSpacing(QTextBrowser* tb) {
	QFontMetrics fm(tb->font());
	int lineHeight = fm.lineSpacing() * StatusStore::instance().getTextSettings().getLineSpacing();

	QTextCursor cursor(tb->document());
	cursor.select(QTextCursor::Document);
	QTextBlockFormat blockFormat = cursor.blockFormat();
	// FixedHeight의 경우 픽셀 단위로 간격을 지정합니다.
	blockFormat.setLineHeight(lineHeight, QTextBlockFormat::FixedHeight);
	cursor.setBlockFormat(blockFormat);
}

/*
	Setting History
*/

void TextViewContainer::saveHistory(HistoryProps& history, const TextFileInfo* fileInfo) {
	if (!fileInfo->fileName.isEmpty()) {
		history.addFileInfo(fileInfo->fileName, fileInfo->pageInfos.value(fileInfo->currentPageIdx).firstPosition, "");
	}
}


/*
	page, file event
*/

bool TextViewContainer::eventFilter(QObject* watched, QEvent* event) {

	if (this->isVisible() == false) {
		return QWidget::eventFilter(watched, event);
	}

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_PageDown) {
			initTextFile(m_fileInfo.fileName, FileUtils::Next);
		}
		else if (keyEvent->key() == Qt::Key_PageUp) {
			initTextFile(m_fileInfo.fileName, FileUtils::Prev);
		}
		else if (keyEvent->key() == Qt::Key_Left) {
			prevPage(&m_fileInfo);
		}
		else if (keyEvent->key() == Qt::Key_Right) {
			nextPage(&m_fileInfo);
		}
		else if (keyEvent->key() == Qt::Key_Delete && !m_fileInfo.fileName.isEmpty()) {
			deleteFile(&m_fileInfo);
		}
		return false;  // 이벤트를 가로채서 처리 완료
	}


	if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint mousePos = static_cast<QWidget*>(watched)->mapTo(this, mouseEvent->pos());

		int centerX = this->width() / 2;

		if (mousePos.x() < centerX) {
			prevPage(&m_fileInfo);
		}
		else {
			nextPage(&m_fileInfo);
		}
	}

	return QWidget::eventFilter(watched, event);
}

void TextViewContainer::deleteFile(const TextFileInfo* fileInfo) {
	QStringList files;
	files.append(fileInfo->fileName);

	emit deleteKeyPressed(files);
}

void TextViewContainer::nextPage(const TextFileInfo* fileInfo) {
	int nextPosition = fileInfo->currentPageIdx;
	bool isSplitView = StatusStore::instance().getTextSettings().isSplitView();

	int increment = isSplitView ? 2 : 1;
	nextPosition += increment;

	if (nextPosition >= fileInfo->pageInfos.size()) {
		if (StatusStore::instance().getTextSettings().isAutoNext()) {
			initTextFile(m_fileInfo.fileName, FileUtils::Next);
			return;
		}
		return;
	}

	ui_QSlider->setValue(nextPosition);
}

void TextViewContainer::prevPage(const TextFileInfo* fileInfo) {
	int prevPosition = fileInfo->currentPageIdx;
	bool isSplitView = StatusStore::instance().getTextSettings().isSplitView();

	int increment = isSplitView ? 2 : 1;
	prevPosition -= increment;

	if (prevPosition < 0 && StatusStore::instance().getTextSettings().isAutoNext()) {
		initTextFile(m_fileInfo.fileName, FileUtils::Prev);
		return;
	}
	else if (prevPosition < 0) {
		prevPosition = 0;
	}

	ui_QSlider->setValue(prevPosition);
}

bool TextViewContainer::changeSplitView() {

	TextSettingProps& setting = StatusStore::instance().getTextSettings();
	bool newSplit = !setting.isSplitView();
	setting.setSplitView(newSplit);
	ui_TextBrowsers[1]->setVisible(newSplit);
	StatusStore::instance().saveTextSettings(setting);
	return newSplit;
}

/*
	page calculation logic
*/

int TextViewContainer::setPage(TextFileInfo* fileInfo, int newPageIdx) {
	if (newPageIdx < 0 || newPageIdx >= fileInfo->pageInfos.size()) {
		return fileInfo->currentPageIdx;
	}

	//잘못된 페이지 보정(직접 바를 움직였을때 등)
	if (StatusStore::instance().getTextSettings().isSplitView()) {
		newPageIdx = newPageIdx - (newPageIdx % 2);
	}

	fileInfo->currentPageIdx = newPageIdx;
	fileInfo->currentPosition = fileInfo->pageInfos.value(newPageIdx).firstPosition;

	for (int i = 0; i < M_TEXT_BROWSER_CNT; i++) {
		if (ui_TextBrowsers[i]->isVisible()) {

			QVector<QString> lines = fileInfo->pageInfos.value(newPageIdx + i).lines;

			ui_TextBrowsers[i]->clear();
			ui_TextBrowsers[i]->setPlainText(lines.join(""));
			ui_TextBrowsers[i]->moveCursor(QTextCursor::Start);

			applyLineSpacing(ui_TextBrowsers[i]);
		}
	}

	saveHistory(StatusStore::instance().getTextHistory(), fileInfo);

	return newPageIdx;
	//testText();
}

QHash<long, TextViewContainer::PageInfo> TextViewContainer::calculatePage(const TextFileInfo* fileInfo, int maxLine, int maxWidth, QTextBrowser* browser) {
	QHash<long, PageInfo> textPages;
	QString line;
	int width = 0;
	QFontMetrics fm(browser->font());
	int pageCnt = 0;
	TextViewContainer::PageInfo page;

	for (long i = 0; i < fileInfo->text.length(); i++) {

		if (page.firstPosition == -1) {
			page.firstPosition = i;
		}

		QChar c = fileInfo->text.at(i);
		line.append(c);
		width += getFontWidth(&fm, c);
		if (c == '\n' || width >= maxWidth) {
			page.lines.append(line);
			line.clear();
			width = 0;

			if (page.lines.size() >= maxLine) {

				textPages.insert(pageCnt++, page);
				page = TextViewContainer::PageInfo();
			}
		}
	}

	if (!line.isEmpty()) {
		page.lines.append(line);
	}

	textPages.insert(pageCnt++, page);

	return textPages;
}
/*
void TextViewContainer::testText() {

	PageInfo p = m_fileInfo.pageInfos.value(m_fileInfo.currentPageIdx);
	QFontMetrics fm(ui_TextBrowsers[0]->font());
	int width = 0;

	for (int i = 0; i < p.lines.size(); i++) {
		int width = 0;
		for (int j = 0; j < p.lines.at(i).length(); j++) {
			QChar c = p.lines.at(i).at(j);
			int w = getFontWidth(&fm, c);
			width += w;
			qDebug() << QString(c) << " : " << w;
		}
		qDebug() << "----------------- " << getMaxWidth(ui_TextBrowsers[0]);
		qDebug() << p.lines.at(i);
		qDebug() << "line width: " << width << " , w2 : " << fm.horizontalAdvance(p.lines.at(i));
	}


}*/


int TextViewContainer::getFontWidth(QFontMetrics* fm, QChar c) {
	if (!m_charWidthCache.contains(c)) {
		int width = fm->horizontalAdvance(c); // 글자의 너비
		//width += fm->rightBearing(c); // 오른쪽 여백

		// 탭 간격을 고려
		if (c == '\t') {
			//   width = fm->horizontalAdvance(' ') * 4; // 예: 탭 간격을 4개의 공백으로 설정
		}

		m_charWidthCache.insert(c, width);  // 캐시에 값 저장
	}
	return m_charWidthCache.value(c);
}


int TextViewContainer::getMaxHeight(QTextBrowser* tb) {
	TextSettingProps settings = StatusStore::instance().getTextSettings();
	QFontMetrics fm(tb->font());
	int lineHeight = fm.lineSpacing() * settings.getLineSpacing();

	return ((tb->height() - (settings.getPadding().top() + settings.getPadding().bottom())) / lineHeight) - 1;  // 세로 방향으로 표시 가능한 줄 수
}
int TextViewContainer::getMaxWidth(QTextBrowser* tb) {
	TextSettingProps settings = StatusStore::instance().getTextSettings();

	return ui_TextBrowsers[0]->viewport()->width() - settings.getPadding().left() - settings.getPadding().right() - 3;
}



const TextViewContainer::TextFileInfo* TextViewContainer::getFileInfo() {
	return &m_fileInfo;
}

/*
	Search
*/

void TextViewContainer::performSearch(QString searchText, const TextFileInfo* fileInfo) {

	for (long i = 0; i < fileInfo->pageInfos.size(); i++) {
		QVector<QString> lines = fileInfo->pageInfos.value(i).lines;
		for (long j = 0; j < lines.size(); j++) {
			QString line = lines.at(j);
			if (line.contains(searchText)) {
				emit searchResultReady(line, i, j);
			}
		}
	}

}