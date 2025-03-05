#include "DeleteFilesDialog.h"

DeleteFilesDialog::DeleteFilesDialog(const QStringList& files, bool isFolder, QWidget* parent)
    : QDialog(parent), m_fileListWidget(new QListWidget(this)), m_deleteFolderCheckBox(new QCheckBox("delete folder with all file", this)),
	m_okButton(new QPushButton("Confirm", this)), m_cancelButton(new QPushButton("Cancel", this))
{
    setWindowTitle("Delete File");
    resize(800, 300); // 창 크기 설정

    // 파일 목록을 QListWidget에 추가하고 각 항목에 체크박스를 설정
    for (const QString& file : files) {
        QListWidgetItem* item = new QListWidgetItem(file, m_fileListWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
    }

    // 레이아웃 설정
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_fileListWidget);
    mainLayout->addWidget(m_deleteFolderCheckBox);
	m_deleteFolderCheckBox->setCheckState(isFolder ? Qt::Checked : Qt::Unchecked);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);

    // 버튼 시그널 연결
    connect(m_okButton, &QPushButton::clicked, this, &DeleteFilesDialog::deleteFileOrFolder);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QStringList DeleteFilesDialog::getSelectedFiles() const {
    QStringList selectedFiles;
    for (int i = 0; i < m_fileListWidget->count(); ++i) {
        QListWidgetItem* item = m_fileListWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            selectedFiles.append(item->text());
        }
    }
    return selectedFiles;
}

QStringList DeleteFilesDialog::getDeletedFiles() const {
	return m_deletedFiles;
}

bool DeleteFilesDialog::isDeleteFolderChecked() const {
    return m_deleteFolderCheckBox->isChecked();
}

void DeleteFilesDialog::deleteFileOrFolder() {
	m_deletedFiles.clear();

    QStringList selectedFiles = getSelectedFiles();
    if (isDeleteFolderChecked()) {
        QString folderPath = QFileInfo(m_fileListWidget->item(0)->text()).absolutePath();
        FileUtils::moveFolderToTrash(folderPath);
        if (!QDir(folderPath).exists()) {
            m_deletedFiles.append(folderPath);
        }
        else {
            QMessageBox::warning(this, "warning", "can not delete  " + folderPath);
        }
    } else {
        for (const QString& file : selectedFiles) {
            FileUtils::moveToTrash(file);
			if (!QFile(file).exists()) {
				m_deletedFiles.append(file);
            }
            else {
                QMessageBox::StandardButton reply = QMessageBox::question(this,
                    "Warning",
                    "Can not delete " + file + "\nDo you want to continue?",
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::No) {
                    break;
                }
            }
        }
    }

	accept();
}
