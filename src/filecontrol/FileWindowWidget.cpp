// HistoryTableWidget.cpp
#include "FileWindowWidget.h"

FileWindowWidget::FileWindowWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void FileWindowWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 리스트 위젯 설정
    ui_listWidget = new QListWidget(this);
    mainLayout->addWidget(ui_listWidget);

    // QLabel 대신 QLineEdit 사용
    ui_lineEdit = new QLineEdit(tr("last action :: "), this);
    ui_lineEdit->setReadOnly(true);  // 읽기 전용으로 설정
    ui_lineEdit->setFrame(false);    // 테두리 제거

    // 스타일 설정
    QString style = "QLineEdit { "
        "  background: transparent; "  // 배경 투명
        "  border: none; "            // 테두리 없음
        "  padding: 2px; "
        "}";
    ui_lineEdit->setStyleSheet(style);

    mainLayout->addWidget(ui_lineEdit);
    // 버튼 레이아웃
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* ui_selectedToggle = new QPushButton(tr("checked All"), this);
    QPushButton* ui_btnMoveFolder = new QPushButton(tr("move folder"), this);
    QPushButton* ui_btnRename = new QPushButton(tr("renames"), this);
    QPushButton* ui_btnClear = new QPushButton(tr("remove"), this);
    QPushButton* ui_sort = new QPushButton(tr("sort"), this);

    buttonLayout->addWidget(ui_selectedToggle);
    buttonLayout->addWidget(ui_btnMoveFolder);
    buttonLayout->addWidget(ui_btnRename);
    buttonLayout->addWidget(ui_btnClear);
    buttonLayout->addWidget(ui_sort);
    buttonLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addLayout(buttonLayout);

    // 시그널/슬롯 연결
    connect(ui_selectedToggle, &QPushButton::clicked, this, [this]() {
		m_isSelectedAll = !m_isSelectedAll;
        for (int i = 0; i < ui_listWidget->count(); i++) {
            ui_listWidget->item(i)->setCheckState(m_isSelectedAll ? Qt::Checked : Qt::Unchecked);
        }
        });

    connect(ui_btnMoveFolder, &QPushButton::clicked, this, &FileWindowWidget::onMoveFolder);
    connect(ui_btnRename, &QPushButton::clicked, this, &FileWindowWidget::onRename);
    connect(ui_btnClear, &QPushButton::clicked, this, [this]() {
		ui_listWidget->clear();
        });
    connect(ui_sort, &QPushButton::clicked, this, &FileWindowWidget::sortItems);

    // 더블클릭 이벤트 연결
    connect(ui_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            QString text = item->text();
            qDebug() << "Double clicked:" << text;
            emit onItemDoubleClick(text);
        }
        });

}

void FileWindowWidget::addItem(const QString& text, bool checked)
{

    for (int i = 0; i < ui_listWidget->count();i++) {
        QListWidgetItem* item = ui_listWidget->item(i);
        QString itemText = item->text();  // 텍스트 가져오기
		if(text == itemText) {
            setLineText(QString("add :: %1").arg(text), "red");
            delete ui_listWidget->takeItem(i); // 아이템 제거
            return;
		}
    }

    QListWidgetItem* item = new QListWidgetItem(ui_listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // 체크박스 기능 활성화
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked); // 체크 상태 설정
    item->setText(text); // 텍스트 설정

    setLineText(QString("add :: %1").arg(text), "blue");

    ui_listWidget->scrollToItem(item);
}

void FileWindowWidget::onMoveFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("폴더 선택"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {

        int successCount = 0;
        int failCount = 0;
        QStringList failedItems;

        // 선택된 아이템들만 이동
        QList<QListWidgetItem*> selectedItems = getCheckedItem();
        if (selectedItems.isEmpty()) {
            setLineText("No selected items", "black");
            return;
        }

        for (QListWidgetItem* item : selectedItems) {
            QString srcPath = item->text();
            QString fileName = QFileInfo(srcPath).fileName();
            QString destPath = QDir(dir).filePath(fileName);

            QFile file(srcPath);
            if (file.rename(destPath)) {
                successCount++;
				// 아이템 텍스트 변경
				item->setText(destPath);
            }
            else {
                failCount++;
                failedItems.append(srcPath);
            }
        }

        setLineText(QString("Move completed, Success: %1, Failed: %2").arg(successCount).arg(failCount), "black");


    }
}
bool FileWindowWidget::hasRenameSpecialChar(QString pattern) {

    // 허용되는 특수문자 패턴을 제거한 후 남은 특수문자 체크
    QString checkPattern = pattern;
    checkPattern.remove("?");
    // <>, <n>, <n|m> 패턴 찾기 및 제거
    QRegularExpression validPattern("<\\d*(?:\\|\\d+)?>");
    checkPattern.replace(validPattern, "");

    // Windows에서 파일명으로 사용할 수 없는 문자들 체크
    QRegularExpression invalidChars("[\\\\/:*?\"<>|]");
    QRegularExpressionMatch invalidMatch = invalidChars.match(checkPattern);

    if (invalidMatch.hasMatch()) {
        QMessageBox::warning(this, "Warning",
            "The file name contains special characters that cannot be used.:\n" + checkPattern);
        return true;
    }
    return false;
}

void FileWindowWidget::setLineText(QString text, QString color)
{
    ui_lineEdit->setText(tr("rename :: ") + text);
    ui_lineEdit->setStyleSheet(QString("QLineEdit { color: %1; background: transparent; border: none; }").arg(color));
}

void FileWindowWidget::onRename()
{
    QList<QListWidgetItem*> selectedItems = getCheckedItem();

    if (selectedItems.isEmpty()) {
		setLineText("No selected items", "black");
        return;
    }

    QString pattern = QInputDialog::getText(this, tr("Rename"), tr("pattern\n? : hold original name\n> : increase number(<padding|start number>)\n ex `test-<2|2>` :: test-02, test-03.... "));
    if (pattern.isEmpty() || hasRenameSpecialChar(pattern)) {
        setLineText("Pattern validation fail", "black");
        return;
    }


    int startIndex = 1;  // 기본 시작값
    int digitCount = 3;  // 기본 자릿수

    // <> 또는 <n> 또는 <n|start> 패턴 찾기
    QRegularExpression rx("<(\\d*)(?:\\|(\\d+))?>");
    QRegularExpressionMatch match = rx.match(pattern);

    if (match.hasMatch()) {
        QString digitStr = match.captured(1);

        // <> 경우 기본값 사용
        if (!digitStr.isEmpty()) {
            digitCount = digitStr.toInt();
        }

        // 시작값이 있으면 설정
        if (match.capturedLength(2) > 0) {
            startIndex = match.captured(2).toInt();
        }
    }

    int currentIndex = startIndex;
	int successCount = 0;
	int failCount = 0;

    for (QListWidgetItem* item : selectedItems) {
        QString oldPath = item->text();
        QFileInfo fileInfo(oldPath);
        QString oldNameWithoutExt = fileInfo.baseName();
        QString extension = fileInfo.suffix();
        QString newName = pattern;

        // ? 패턴 처리 - 원본 이름 유지
        if (pattern.contains("?")) {
            newName.replace("?", oldNameWithoutExt);
        }

        // <>, <n> 또는 <n|start> 패턴 처리
        if (match.hasMatch()) {
            QString paddedNumber = QString("%1").arg(currentIndex++, digitCount, 10, QChar('0'));
            newName.replace(rx, paddedNumber);
        }

        // 확장자 추가
        if (!extension.isEmpty()) {
            newName += "." + extension;
        }

        // 전체 경로 생성
        QString newPath = QFileInfo(oldPath).dir().filePath(newName);

		if (QFile::rename(oldPath, newPath)) {
            item->setText(newPath);
			successCount++;
        }
        else {
			failCount++;
        }

    }

    setLineText(QString("Rename completed Success: %1 , Failed: %2").arg(successCount).arg(failCount), "black");

}

QList<QListWidgetItem*> FileWindowWidget::getCheckedItem() {
	QList<QListWidgetItem*> checkedItems;
	for (int i = 0; i < ui_listWidget->count(); i++) {
		QListWidgetItem* item = ui_listWidget->item(i);
		if (item->checkState() == Qt::Checked) {
			checkedItems.append(item);
		}
	}
	return checkedItems;
}

void FileWindowWidget::clearLineEditText() {
    ui_lineEdit->clear();
}

void FileWindowWidget::sortItems()
{
    // 아이템의 텍스트와 체크 상태를 맵으로 저장
    QMap<QString, bool> itemStates;
    for (int i = 0; i < ui_listWidget->count(); i++) {
        QListWidgetItem* item = ui_listWidget->item(i);
        itemStates[item->text()] = (item->checkState() == Qt::Checked);
    }

    // 텍스트 목록 정렬
    QStringList items = itemStates.keys();
    FileUtils::sortByWindow(items);

    // 리스트 위젯 초기화 후 정렬된 아이템 다시 추가
    ui_listWidget->clear();
    for (const QString& text : items) {
        QListWidgetItem* item = new QListWidgetItem(ui_listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(itemStates[text] ? Qt::Checked : Qt::Unchecked);
        item->setText(text);
    }
}