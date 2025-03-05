#include "FileWindowContainer.h"

FileWindowContainer::FileWindowContainer(QWidget* parent)
    : QDialog(parent)
{

    //창 설정.
    QSettings settings("SizzViewer", "SizzViewer-FileWindowContainer");
    QRect geom = settings.value("geometry", QRect(100, 100, 300, 800)).toRect();

    // 현재 화면의 사용 가능한 영역 가져오기
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();

    // 창이 화면 밖에 있는지 확인
    if (!screenGeometry.contains(geom.topLeft()) || !screenGeometry.contains(geom.bottomRight())) {
        QPoint parentPos = parent->mapToGlobal(QPoint(0, 0)); // 부모 위젯의 전역 좌표

        int x = parentPos.x() - geom.width();

        if (x < screenGeometry.left()) {
            x = parentPos.x() + parent->width();
        }

        // y 위치는 부모 위젯과 같은 높이로
        int y = parentPos.y();
        geom.moveTopLeft(QPoint(x, y));
    }

    setGeometry(geom);

    setWindowTitle(tr("History Tables"));

    ui_mainLayout = new QVBoxLayout(this);

    FileWindowWidget* widgetf5 = new FileWindowWidget(this);
    ui_tableWidgets.append(widgetf5);
    ui_mainLayout->addWidget(widgetf5);
    FileWindowWidget* widgetf6 = new FileWindowWidget(this);
    ui_tableWidgets.append(widgetf6);
    ui_mainLayout->addWidget(widgetf6);

    FileWindowWidget* widgetf7 = new FileWindowWidget(this);
    ui_tableWidgets.append(widgetf7);
    ui_mainLayout->addWidget(widgetf7);


    FileWindowWidget* widgetf8 = new FileWindowWidget(this);
    ui_tableWidgets.append(widgetf8);
    ui_mainLayout->addWidget(widgetf8);


    setLayout(ui_mainLayout);

    for (int i = 0; i < ui_tableWidgets.size(); i++) {
        connect(ui_tableWidgets[i], &FileWindowWidget::onItemDoubleClick,
            this, [this, i](const QString& fileName) {
                emit onItemDoubleClicked(i, fileName);
            });
    }

}

FileWindowContainer::~FileWindowContainer()
{
    QSettings settings("SizzViewer", "SizzViewer-FileWindowContainer");
    settings.setValue("geometry", this->geometry());
}

FileWindowWidget* FileWindowContainer::getTableWidget(int index)
{
    if (index >= 0 && index < ui_tableWidgets.size()) {
        return ui_tableWidgets[index];
    }
    return nullptr;
}

void FileWindowContainer::appendFile(int keyEvent, QString fileName)
{
    if (fileName.isEmpty()) {
        return;
    }

	for (int i = 0; i < ui_tableWidgets.size(); i++) {
        ui_tableWidgets[i]->clearLineEditText();
	}

    if (keyEvent == Qt::Key_F5) {
		qDebug() << "F5 : " << fileName;
		ui_tableWidgets[0]->addItem(fileName, true);
	}
	else if (keyEvent == Qt::Key_F6) {
        qDebug() << "F6 : " << fileName;
		ui_tableWidgets[1]->addItem(fileName, true);
	}
	else if (keyEvent == Qt::Key_F7) {
        qDebug() << "F7 : " << fileName;
		ui_tableWidgets[2]->addItem(fileName, true);
	}
    else if (keyEvent == Qt::Key_F8) {
        qDebug() << "F8 : " << fileName;

        ui_tableWidgets[3]->addItem(fileName, true);
    }

}
