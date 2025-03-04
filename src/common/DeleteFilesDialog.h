#ifndef DELETEFILESDIALOG_H
#define DELETEFILESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "FileUtils.h"


class DeleteFilesDialog : public QDialog {
    Q_OBJECT

public: 
    explicit DeleteFilesDialog(const QStringList& files, bool isFolder, QWidget* parent = nullptr);
    QStringList getSelectedFiles() const;
    bool isDeleteFolderChecked() const;
    QStringList getDeletedFiles() const;

private: //variable
    QListWidget* m_fileListWidget;
    QCheckBox* m_deleteFolderCheckBox;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QStringList m_deletedFiles;

private: //method
    void deleteFileOrFolder();

};

#endif // DELETEFILESDIALOG_H