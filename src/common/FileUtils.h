#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QStringList>
#include "ZipArchiveManager.h"
#include <QInputDialog>
#include < QLineEdit>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shlobj.h>
#endif

#ifdef Q_OS_MAC
#import <Foundation/Foundation.h>
#endif

class FileUtils
{
public:

	struct viewerFile {
		bool isArchive;
		QString fileName;
		QString archiveName;
		QByteArray fileDataCache;
		int size;
		int currentIndex;
		QStringList fileList;
	};

	enum MoveMode {
		None,
		Next,
		Last,
		NextFolder,
		Prev,
		First,
		PrevFolder,
		PrevFolderLastFile
	};

	enum SupportType {
		TEXT,
		IMAGE,
		ZIP
	};

	FileUtils();
	static void sortByWindow(QStringList& fileList);
	static QStringList getFileList(QString currentFile, SupportType type);
	static bool isSupportSuffix(QString currentFile, SupportType type);
	static QString moveFolder(QString fileName, MoveMode moveMode, SupportType type);
	static QString findFileInSubDir(QString fileName);
	static void moveToTrash(QString filePath);
	static void moveFolderToTrash(QString folderPath);
	static bool isArchivePath(QString filePath);
	static QList<FileUtils::viewerFile> extractFileListBy(QString filePath, FileUtils::MoveMode moveMode, FileUtils::SupportType type, bool isPairPage = false);
	static QString renameFile(QString currentFile, QWidget* parent);
	static QString renameFolder(QString currentFile, QWidget* parent);
	static void setAddEmptyPage(bool isImageFirstEmpty);
	static bool isAddEmptyPage();


private:
	static QList<FileUtils::viewerFile> extractFileInfos(QString filePath, FileUtils::MoveMode moveMode, FileUtils::SupportType type, bool isPairPage);
	static QList<FileUtils::viewerFile> extractArchiveInfos(QString filePath, FileUtils::MoveMode moveMode, FileUtils::SupportType type, bool isPairPage);

	static bool g_isImageFirstEmpty;
};

#endif 