#ifndef ZPIARCHIVEMANAGER_H
#define ZPIARCHIVEMANAGER_H

#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QMap>
#include "FileUtils.h"
#include "../thirdparty/miniz/miniz.h"
#include <QDebug>

class ZipArchiveManager {

public:
    struct ZipCache {
        QString zipPath;
        QMap<QString, QByteArray> fileDataCache;
        QStringList fileList;
    };

    static ZipArchiveManager& instance() {
        static ZipArchiveManager instance;
        return instance;
    }
    static QString ARCHIVE_FILE_PREFIX;

    bool openArchive(const QString& zipPath);
    QByteArray getFileData(const QString& fileName);
    QStringList getFileList();
    QString getZipPath();
    std::shared_ptr<ZipCache> getArchive();  
    void closeArchive();  

private:
    ZipArchiveManager();
    ZipArchiveManager(const ZipArchiveManager&) = delete;

    struct ZipCache;  // 전방 선언
    std::shared_ptr<ZipCache> m_currentArchive;

};

#endif 