#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QStringList>
#include "ZipArchiveManager.h"

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

	struct SzViewerFile {
		bool isArchive;
		QString fileName;
		QByteArray fileDataCache;
		int size;
		int currentIndex;
	};

	enum MoveMode {
		None,
		Next,
		Last,
		NextFolder,
		Prev,
		First,
		PrevFolder
	};

	enum SupportType {
		TEXT,
		IMAGE,
		ZIP
	};

	FileUtils();
	static void sortByWindow(QStringList& fileList);
	static QString MoveFile(QStringList& fileList, QString currentFile, int moveIdx);
	static QStringList getFileList(QString currentFile, SupportType type);
	static bool isSupportSuffix(QString currentFile, SupportType type);
	static QString moveFolder(QString fileName, MoveMode moveMode, SupportType type);
	static QString findFileInSubDir(QString fileName);
	static void moveToTrash(QString filePath);
	static void moveFolderToTrash(QString folderPath);
	static bool isArchivePath(QString filePath);
	static QList<FileUtils::SzViewerFile> extractFileListBy(QString filePath, FileUtils::MoveMode moveMode, FileUtils::SupportType type, bool isPairPage = false);
};

#endif 