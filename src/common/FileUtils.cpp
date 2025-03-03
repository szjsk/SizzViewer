#include "FileUtils.h"
#include <QCollator>
#include <QFileInfo>
#include <QDir>

FileUtils::FileUtils() {

}

void FileUtils::sortByWindow(QStringList &fileList) {
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(fileList.begin(), fileList.end(),
        [&collator](const QString& s1, const QString& s2) {
            return collator.compare(s1, s2) < 0;
        });
}

QString FileUtils::MoveFile(QStringList &fileList, QString currentFile, int moveIdx) {

	if (fileList.isEmpty() || currentFile.isEmpty()) {
        return QString();
	}

    QFileInfo fileInfo(currentFile);
    QDir dir = fileInfo.dir();

	fileList.indexOf(currentFile);

    int currentIndex = fileList.indexOf(currentFile);
    int idx = currentIndex + moveIdx;
    if (currentIndex > -1 && idx < fileList.size() && idx > -1) {
        return isArchivePath(fileList.at(idx)) ? fileList.at(idx) : dir.absoluteFilePath(fileList.at(idx));
    }

    return QString();

}

QStringList FileUtils::getFileList(QString currentFile, SupportType type) {
	QFileInfo fileInfo(currentFile);
	QDir dir = fileInfo.dir();
	QStringList fileList = dir.entryList(QDir::Files);
    QStringList newFileBySupprot;
	for (int i = 0; i < fileList.size(); i++) {
		if (isSupportSuffix(dir.absoluteFilePath(fileList[i]), type)) {
			newFileBySupprot.append(dir.absoluteFilePath(fileList[i]));
		}
	}
	sortByWindow(newFileBySupprot);
	return newFileBySupprot;
}

bool FileUtils::isSupportSuffix(QString currentFile, SupportType type) {
    QFileInfo fileInfo(currentFile);

    QString suffix = fileInfo.suffix().toLower();

    if (type == SupportType::IMAGE && (suffix == "jpg" || suffix == "jpeg" || suffix == "png" || suffix == "bmp" || suffix == "gif" || suffix == "webp"
		|| suffix == "tiff" || suffix == "tif" || suffix == "svg" || suffix == "icns" || suffix == "ico" || suffix == "tga" || suffix == "wbmp"
		)) {
		return true;
	}
	else if (type == SupportType::TEXT && (suffix == "txt")) {
		return true;
	}
    else if (type == SupportType::ZIP && (suffix == "zip")) {
        return true;
    }

    return false;
}


QString FileUtils::findFileInSubDir(QString fileName) {
    QFileInfo fileInfo(fileName);

    if (!fileInfo.isDir()) {
		return fileName;
    }

    QDir folder(fileName);
    QStringList fileNames = folder.entryList(QDir::Files, QDir::Name);
    // 확장자 필터링
    QStringList filteredFileNames;
    for (const QString& name : fileNames) {
        if (FileUtils::isSupportSuffix(name, FileUtils::IMAGE) || FileUtils::isSupportSuffix(name, FileUtils::TEXT)) {
            filteredFileNames.append(name);
        }
    }

    if (filteredFileNames.isEmpty()) {
        QStringList folderNames = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        if (folderNames.isEmpty()) {
            return QString();
        }

        FileUtils::sortByWindow(folderNames);

        return findFileInSubDir(folder.absoluteFilePath(folderNames[0]));
    }

    FileUtils::sortByWindow(filteredFileNames);

    return folder.absoluteFilePath(filteredFileNames[0]);

}

QString FileUtils::moveFolder(QString fileName, MoveMode moveMode, SupportType type) {
    
    // 현재 파일이 속한 폴더
    QFileInfo currentFileInfo(fileName);
    QDir currentFolder = currentFileInfo.isDir()? QDir(fileName) : currentFileInfo.dir();

    // 부모 폴더로 이동
    QDir parentDir = currentFolder;
    if (!parentDir.cdUp()) {
        return QString();
    }

    // 부모 폴더 내의 모든 서브 폴더 리스트 (알파벳 순)
    QStringList folderNames = parentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if (folderNames.isEmpty()) {
        return QString();
    }

	sortByWindow(folderNames);

    // 현재 폴더 이름 및 인덱스 결정
    QString currentFolderName = currentFolder.dirName();
    int index = folderNames.indexOf(currentFolderName);
    if (index < 0) {
        return QString();
    }

    // next 또는 prev 폴더 인덱스 계산
    int nextIndex = -1;
    if (moveMode == FileUtils::MoveMode::NextFolder) {
        nextIndex = index + 1;
    }
    else if (moveMode == FileUtils::MoveMode::PrevFolder) {
        nextIndex = index - 1;
    }

    // 범위를 벗어나면 아무것도 하지 않음
    if (nextIndex < 0 || nextIndex >= folderNames.size()) {
        return QString();
    }

    // 다음(또는 이전) 폴더 경로 결정
    QString nextFolderName = folderNames.at(nextIndex);
    QDir nextFolder(parentDir.absoluteFilePath(nextFolderName));

    QStringList fileNames = nextFolder.entryList(QDir::Files, QDir::Name);

    QStringList newFilePaths;
    for (int i = 0; i < fileNames.size(); i++) {
        if (isSupportSuffix(nextFolder.absoluteFilePath(fileNames[i]), type)) {
            newFilePaths.append(nextFolder.absoluteFilePath(fileNames[i]));
        }
    }
    sortByWindow(newFilePaths);

    if (!newFilePaths.isEmpty()) {
        int idx = moveMode == MoveMode::NextFolder ? 0 : newFilePaths.size() - 1;
        QString file = nextFolder.absoluteFilePath(newFilePaths[idx]);
        return file;
	}
	else {
        return moveFolder(nextFolder.absolutePath(), moveMode, type);
	}
    return QString();
}

QList<FileUtils::SzViewerFile> FileUtils::extractFileListBy(QString filePath, FileUtils::MoveMode moveMode, FileUtils::SupportType type, bool isPairPage) {

	if (filePath.isEmpty()) {
		return QList<FileUtils::SzViewerFile>();
	}

    if (isArchivePath(filePath)) {
		ZipArchiveManager& zipManager = ZipArchiveManager::instance();
        QStringList list = zipManager.getFileList();

        int currentIndex = list.indexOf(filePath);

        if (moveMode == Next) {
            currentIndex += 1;
		}
		else if (moveMode == Prev) {
			currentIndex -= ((isPairPage == true) ? 2 : 1);
		}
		else if (moveMode == First) {
			currentIndex = 0;
		}
		else if (moveMode == Last) {
            currentIndex = (list.size() % 2) == 0 && isPairPage ? list.size() - 2 : list.size() - 1;
		}
		else if (moveMode == NextFolder) {//압축파일일때는 폴더가 아닌 다음 파일로 이동.
            return extractFileListBy(zipManager.getZipPath(), FileUtils::MoveMode::Next, type, isPairPage);
		}
		else if (moveMode == PrevFolder) {
            return extractFileListBy(zipManager.getZipPath(), FileUtils::MoveMode::Prev, type, isPairPage);
        }

		int size = isPairPage ? 2 : 1;
        QList<FileUtils::SzViewerFile> result;
        for (int i = 0; i < size; i++) {
            int currentIdx = currentIndex + i;  

            if (currentIdx < 0 || currentIdx >= list.size()) {
				continue;
			}

            QString fileName = list.at(currentIndex + i);
            SzViewerFile file;
            file.isArchive = true;
            file.fileName = fileName;
            file.fileDataCache = zipManager.getFileData(list.at(currentIndex + i));
			file.size = zipManager.getFileList().size();
			file.currentIndex = currentIndex + i;
			result.append(file);
        }

        return result;
	}
	else {
		qDebug() << "extractFileListBy : " << filePath;

		QStringList list = getFileList(filePath, type);
        int currentIndex = list.indexOf(filePath);

        if (moveMode == Next) {
            currentIndex += 1;
        }
        else if (moveMode == Prev) {
            currentIndex -= (isPairPage) ? 2 : 1;
        }
        else if (moveMode == First) {
            currentIndex = 0;
        }
        else if (moveMode == Last) {
            currentIndex = (list.size() % 2) == 0 && isPairPage ? list.size() - 2 : list.size() - 1;
        }
        else if (moveMode == NextFolder) {//압축파일일때는 폴더가 아닌 다음 파일로 이동.
			QString newFilePath = moveFolder(filePath, FileUtils::MoveMode::NextFolder, type);
            return extractFileListBy(newFilePath, FileUtils::MoveMode::First, type, isPairPage);
        }
        else if (moveMode == PrevFolder) {
            QString newFilePath = moveFolder(filePath, FileUtils::MoveMode::PrevFolder, type);
            return extractFileListBy(newFilePath, FileUtils::MoveMode::First, type, isPairPage);
        }

        int size = isPairPage ? 2 : 1;
        QList<FileUtils::SzViewerFile> result;
        for (int i = 0; i < size; i++) {

            int currentIdx = currentIndex + i;
            if (currentIdx < 0 || currentIdx >= list.size()) {           
                continue;
            }
            QString currentFilePath = list.at(currentIdx);
            QFile qFile(currentFilePath);
            SzViewerFile file;

            if (qFile.open(QIODevice::ReadOnly)) {
                file.isArchive = false;
                file.fileName = currentFilePath;
                file.fileDataCache = qFile.readAll();
                file.size = list.size();
                file.currentIndex = currentIndex + i;
                qFile.close();  // 파일 닫기
                result.append(file);
            }
        }

        return result;

	}
}

bool FileUtils::isArchivePath(QString filePath) {
	return filePath.startsWith(ZipArchiveManager::ARCHIVE_FILE_PREFIX);
}


void FileUtils::moveToTrash(QString filePath) {
#ifdef Q_OS_WIN
    // Windows에서 파일을 휴지통으로 이동
    QString nativeFilePath = QDir::toNativeSeparators(filePath);
    wchar_t* file = new wchar_t[nativeFilePath.length() + 2];
    nativeFilePath.toWCharArray(file);
    file[nativeFilePath.length()] = 0;
    file[nativeFilePath.length() + 1] = 0;

    SHFILEOPSTRUCT fileOp;
    memset(&fileOp, 0, sizeof(SHFILEOPSTRUCT));
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = file;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

    SHFileOperation(&fileOp);

    delete[] file;
#elif defined(Q_OS_MAC)
    // macOS에서 파일을 휴지통으로 이동
    NSString* nsFilePath = [NSString stringWithUTF8String : filePath.toUtf8().constData()];
    NSURL* fileURL = [NSURL fileURLWithPath : nsFilePath];
    [[NSFileManager defaultManager]trashItemAtURL:fileURL resultingItemURL : nil error : nil];
#else
    // 기타 플랫폼에서는 파일을 삭제
    QFile::remove(filePath);
#endif
}

void FileUtils::moveFolderToTrash(QString folderPath) {
#ifdef Q_OS_WIN
    // Windows에서는 폴더 전체를 한번에 휴지통으로 이동
    QString nativeFilePath = QDir::toNativeSeparators(folderPath);
    wchar_t* folder = new wchar_t[nativeFilePath.length() + 2];
    nativeFilePath.toWCharArray(folder);
    folder[nativeFilePath.length()] = 0;
    folder[nativeFilePath.length() + 1] = 0;

    SHFILEOPSTRUCT fileOp;
    memset(&fileOp, 0, sizeof(SHFILEOPSTRUCT));
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = folder;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    SHFileOperation(&fileOp);
    delete[] folder;

#elif defined(Q_OS_MAC)
    // macOS에서는 폴더 전체를 한번에 휴지통으로 이동
    NSString* nsFolderPath = [NSString stringWithUTF8String : folderPath.toUtf8().constData()];
    NSURL* folderURL = [NSURL fileURLWithPath : nsFolderPath];
    [[NSFileManager defaultManager]trashItemAtURL:folderURL resultingItemURL : nil error : nil];

#else
    // 기타 플랫폼
    QDir dir(folderPath);
    dir.removeRecursively();
#endif
}