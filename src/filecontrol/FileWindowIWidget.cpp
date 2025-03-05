// HistoryTableWidget.cpp
#include "FileWindowIWidget.h"

FileWindowIWidget::FileWindowIWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void FileWindowIWidget::setupUI()
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

    ui_selectedToggle = new QPushButton(tr("전체선택/해제"), this);
    ui_btnMoveFolder = new QPushButton(tr("폴더이동"), this);
    ui_btnRename = new QPushButton(tr("이름변경"), this);
    ui_btnDelete = new QPushButton(tr("파일삭제"), this);
    ui_btnClear = new QPushButton(tr("제거"), this);
    buttonLayout->addWidget(ui_selectedToggle);
    buttonLayout->addWidget(ui_btnMoveFolder);
    buttonLayout->addWidget(ui_btnRename);
    buttonLayout->addWidget(ui_btnDelete);
    buttonLayout->addWidget(ui_btnClear);
    buttonLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addLayout(buttonLayout);

    // 시그널/슬롯 연결
    connect(ui_selectedToggle, &QPushButton::clicked, this, [this]() {
		m_isSelectedAll = !m_isSelectedAll;
        for (int i = 0; i < ui_listWidget->count(); i++) {
            ui_listWidget->item(i)->setCheckState(m_isSelectedAll ? Qt::Checked : Qt::Unchecked);
        }
        });

    connect(ui_btnMoveFolder, &QPushButton::clicked, this, &FileWindowIWidget::onMoveFolder);
    connect(ui_btnRename, &QPushButton::clicked, this, &FileWindowIWidget::onRename);
    connect(ui_btnDelete, &QPushButton::clicked, this, &FileWindowIWidget::onDelete);
    connect(ui_btnClear, &QPushButton::clicked, this, [this]() {
		ui_listWidget->clear();
        });


    // 더블클릭 이벤트 연결
    connect(ui_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            QString text = item->text();
            qDebug() << "Double clicked:" << text;
        }
        });

}

void FileWindowIWidget::addItem(const QString& text, bool checked)
{

    for (int i = 0; i < ui_listWidget->count();i++) {
        QListWidgetItem* item = ui_listWidget->item(i);
        QString itemText = item->text();  // 텍스트 가져오기
		if(text == itemText) {
            ui_lineEdit->setText(tr("remove :: ") + itemText);
            ui_lineEdit->setStyleSheet("QLineEdit { color: red; background: transparent; border: none; }");
            delete ui_listWidget->takeItem(i); // 아이템 제거
            return;
		}
    }

    QListWidgetItem* item = new QListWidgetItem(ui_listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // 체크박스 기능 활성화
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked); // 체크 상태 설정
    item->setText(text); // 텍스트 설정
    ui_lineEdit->setText(tr("append :: ") + text);
    ui_lineEdit->setStyleSheet("QLineEdit { color: blue; background: transparent; border: none; }");

    ui_listWidget->scrollToItem(item);
}

void FileWindowIWidget::onMoveFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("폴더 선택"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {

		qDebug() << "폴더 이동: " << dir;   

        //emit folderMoved(dir);
    }
}


void FileWindowIWidget::onRename()
{
    QList<QListWidgetItem*> items = ui_listWidget->selectedItems();
    for (int i = 0; i < items.size();i++) {
		qDebug() << "onRename " << items.at(i);
    }

	qDebug() << "onRename";
	//QList<QListWidgetItem*> items = m_listWidget->selectedItems();
	//if (items.isEmpty()) {
	//	return;
	//}
	//QListWidgetItem* item = items.first();
	//HistoryCheckBoxItem* checkBox = qobject_cast<HistoryCheckBoxItem*>(m_listWidget->itemWidget(item));
	//if (checkBox == nullptr) {
	//	return;
	//}
	//QString oldName = checkBox->text();
	//QString newName = QInputDialog::getText(this, tr("이름 변경"), tr("새로운 이름을 입력하세요."), QLineEdit::Normal, oldName);
	//if (!newName.isEmpty()) {
	//	checkBox->setText(newName);
	//	//emit itemRenamed(oldName, newName);
	//}
}

void FileWindowIWidget::onDelete()
{
	qDebug() << "onDelete";
}


void FileWindowIWidget::clearLineEditText() {
    ui_lineEdit->clear();
}