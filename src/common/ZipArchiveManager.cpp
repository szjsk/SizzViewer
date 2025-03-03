#include "ZipArchiveManager.h"

QString ZipArchiveManager::ARCHIVE_FILE_PREFIX = "archive:://";

ZipArchiveManager::ZipArchiveManager() {
}

// ZipArchiveManager.cpp
bool ZipArchiveManager::openArchive(const QString& zipPath) {
	closeArchive();
    mz_zip_archive zip = {};
    if (!mz_zip_reader_init_file(&zip, zipPath.toUtf8().constData(), 0)) {
        return false;
    }

    auto cache = std::make_shared<ZipCache>();
    cache->zipPath = zipPath;

    int numFiles = (int)mz_zip_reader_get_num_files(&zip);
    for (int i = 0; i < numFiles; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip, i, &file_stat))
            continue;

        QString fileName = QString::fromLocal8Bit(file_stat.m_filename);
        if (FileUtils::isSupportSuffix(fileName, FileUtils::IMAGE) ||
            FileUtils::isSupportSuffix(fileName, FileUtils::TEXT)) {

            size_t size;
            void* data = mz_zip_reader_extract_file_to_heap(&zip, file_stat.m_filename, &size, 0);
            if (data) {
				fileName = ARCHIVE_FILE_PREFIX + fileName;
                QByteArray byteArray((char*)data, size);  // 데이터를 복사하여 QByteArray 생성

                cache->fileDataCache[fileName] = byteArray;
                cache->fileList.append(fileName);
                free(data);
            }
        }
    }
    
    mz_zip_reader_end(&zip);
    
	FileUtils::sortByWindow(cache->fileList);
    m_currentArchive = cache;
    return true;
}

QByteArray ZipArchiveManager::getFileData(const QString& fileName) {
    if (!m_currentArchive) return QByteArray();
    return m_currentArchive->fileDataCache.value(fileName);
}

QStringList ZipArchiveManager::getFileList() {
    if (!m_currentArchive) return QStringList();
    return m_currentArchive->fileList;
}

QString ZipArchiveManager::getZipPath() {
    if (!m_currentArchive) return QString();
    return m_currentArchive->zipPath;
}

std::shared_ptr<ZipArchiveManager::ZipCache> ZipArchiveManager::getArchive() {
    return m_currentArchive;
}

void ZipArchiveManager::closeArchive() {
    m_currentArchive.reset();  // shared_ptr가 자동으로 메모리 해제
}