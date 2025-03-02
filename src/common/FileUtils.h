#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QStringList>

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
	enum SupportType {
		TEXT,
		IMAGE,
	};

	enum MoveMode {
		NextFolder,
		PrevFolder,
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
};

#endif 